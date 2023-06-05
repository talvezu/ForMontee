#include "TasksFromConfig.h"
#include "DB.h"
#include "InteractiveTask.h"
#include "NetFunctionFactory.h"
#include "PeriodicTask.h"
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "global_settings.h"
#include "DashboardDisplay.h"
#include <atomic>
#include <memory>
#include <random>

using in_out_map =
    map<string, map<string, shared_ptr_thread_safe_sample_queue>>;


//template<>
//std::shared_ptr<utils::DB<uint32_t, float>> utils::FunctionPointerHolder<uint32_t, float>::static_ptr = db;


in_out_map queues_map;

std::map<std::string, shared_ptr<InteractiveTask<uint32_t, float>>>
    InteractiveTasks;

float execute_net_function(
    std::map<std::string, std::shared_ptr<NetFunction>> net_functions,
    uint32_t engine,
    float value)
{
    if (settings::logging::active)
        std::cout << "execute_net_function called for engine:" << engine
                  << " with value: " << value << "\n";

    /*to do improve, no point in searching over again*/
    for (auto net_func : net_functions)
    {
        if ( "dashboard" == net_func.first )
        {
            my_opencv::DashBoard dashb;
            dashb.draw_dashboard();
        }
        else
        {
            for (auto motor: net_func.second->get_motors())
            {
                if (motor == engine)
                {
                    if (settings::logging::active)
                        cout << net_func.first << " over " << motor << " called"
                             << "\n";
                    return net_func.second->operator()(value);
                }
            }
        }
    }
    return value;
}

//float execute_auto_tune_function(std::shared_ptr<std::vector<bool>> inc_dec_array, uint32_t engine, float value)
float execute_auto_tune_function(
    std::shared_ptr<std::map<uint32_t, bool>> inc_dec_hash,
    uint32_t engine,
    float value)
{
    if (settings::logging::active)
        std::cout << "execute_auto_tune_function callback called for engine:"
                  << engine << " with value: " << value << "\n";
    if (value < 0.99f && value > 0.01)
        if (inc_dec_hash->at(engine))
            return value *= 1.01f;
        else
            return value *= 0.99f;
    else
    {
        if (inc_dec_hash->at(engine))
            value -= 0.01f;
        else
            value += 0.01f;

        (*inc_dec_hash)[engine] = !(*inc_dec_hash)[engine];
    }
    return value;
}

float execute_random_noise(uint32_t loop, float value)
{

    if (settings::logging::active)
        std::cout << "execute_random_noise callback called for loop:" << loop
                  << " with value: " << value << "\n";


    auto normalDistribution = [](int base, int dev) {
        auto randomFunc =
            [distribution_ = std::normal_distribution<float>(base, dev),
             random_engine_ = std::mt19937{std::random_device{}()}]() mutable {
                return distribution_(random_engine_);
            };
        return randomFunc;
    };

    auto randomNumberBetween = [](int low, int high) {
        auto randomFunc =
            [distribution_ = std::uniform_int_distribution<int>(low, high),
             random_engine_ = std::mt19937{std::random_device{}()}]() mutable {
                return distribution_(random_engine_);
            };
        return randomFunc;
    };

    //static auto noise = randomNumberBetween(0, 100);
    //return  value*(1 + (noise() - 50.0f)/1000.0f);
    static auto noise = normalDistribution(0, 10);
    return noise() / 1000.0f + value;
}

/**
 * Main function for the current project
*/
void tasks_from_config_impl(workflow<float> &work_flow)
{
    auto ForMainPQ = std::make_unique<PQueue<PriorityQueueElement>>();
    work_flow.load_work();

    auto db = utils::get_db<uint32_t, float>();
    if (settings::use_shared_memory::active)
        db->init_sm("db_shared_memory");

    for (auto item : work_flow.get_tasks())
    {
        auto net_func = item.second;
        std::cout << net_func->task_name << "\n";

        std::vector<int> motors;
        for (auto motor : net_func->target_motors)
        {
            motors.push_back(motor);
        }
        std::vector<float> vf{};

        auto elem = std::make_shared<PriorityQueueElement>(
            motors,
            net_func->times,
            (net_func->values.has_value()) ? net_func->values.value() : vf,
            (net_func->delta_values.has_value())
                ? net_func->delta_values.value()
                : vf,
            net_func->delta_in_milliseconds);

        ForMainPQ->push(elem);
    }

    //active_net_functions
    const auto &actions = work_flow.get_net_actions();
    for (const auto &net_action : actions)
    {
        NetFunctionFactory::supportNetFunction(net_action.second.action);
    }

    const auto &action_items = work_flow.get_net_actions();
    const std::map<std::string, std::shared_ptr<NetFunction>> &net_functions =
        NetFunctionFactory::get_active_net_functions();

    for (auto &[net_function_name, net_action_details] : action_items)
    {
        auto net_func = net_functions.at(net_action_details.action);
        if (net_func)
        {
            net_func->set_motors(net_action_details.motors);
        }
        cout << net_action_details.action << "\n";
    }

    std::map<uint32_t, float> new_engines;
    auto inc_dec_hash = std::make_shared<
        std::map<uint32_t,
                 bool>>(); /*make_unique does not work for some reason*/
    for (auto motor_instance : work_flow.get_all_motors())
    {
        new_engines[motor_instance] = 0.5f;
        (*inc_dec_hash)[motor_instance] = true;
    }
    db->set_component_db("Engines", new_engines);
    db->set_component_db("PositionLoop", std::map<uint32_t, float>{{1, 1.0f}});


    auto end_task = std::make_shared<std::atomic<bool>>(false);

    auto net_func_callback = std::bind(&execute_net_function,
                                       net_functions,
                                       std::placeholders::_1,
                                       std::placeholders::_2);

    /**********/
    //Engines //
    /**********/
    auto engines_get_db_values_callback =
        std::bind(&utils::DB<uint32_t, float>::get_component_db,
                  db.get(),
                  std::string("Engines"));
    auto engines_set_db_values_callback =
        std::bind(&utils::DB<uint32_t, float>::set_component_db,
                  db.get(),
                  std::string("Engines"),
                  std::placeholders::_1);
    auto engines_execute_auto_tune_function =
        std::bind(&execute_auto_tune_function,
                  std::move(inc_dec_hash),
                  std::placeholders::_1,
                  std::placeholders::_2);
    InteractiveTasks.emplace("Engines",
                             std::make_shared<InteractiveTask<uint32_t, float>>(
                                 std::move(std::string("Engines")),
                                 nullptr,
                                 end_task,
                                 10,
                                 engines_execute_auto_tune_function,
                                 engines_get_db_values_callback,
                                 engines_set_db_values_callback));

    /**********/
    //  main  //
    /**********/
    InteractiveTasks.emplace("Main",
                             std::make_shared<InteractiveTask<uint32_t, float>>(
                                 std::move(std::string("Main")),
                                 std::move(ForMainPQ),
                                 end_task,
                                 100,
                                 net_func_callback,
                                 engines_get_db_values_callback,
                                 engines_set_db_values_callback));

    /****************/
    // PositionLoop //
    /****************/
    auto loop_get_db_values_callback =
        std::bind(&utils::DB<uint32_t, float>::get_component_db,
                  db.get(),
                  std::string("PositionLoop"));
    auto loop_set_db_values_callback =
        std::bind(&utils::DB<uint32_t, float>::set_component_db,
                  db.get(),
                  std::string("PositionLoop"),
                  std::placeholders::_1);
    InteractiveTasks.emplace("PositionLoop",
                             std::make_shared<InteractiveTask<uint32_t, float>>(
                                 std::string("Loop"),
                                 nullptr,
                                 end_task,
                                 4,
                                 execute_random_noise,
                                 loop_get_db_values_callback,
                                 loop_set_db_values_callback));

    std::this_thread::sleep_for(5000ms);

    end_task->store(true);

    for (auto &[_, t] : InteractiveTasks)
    {
        t->join();
    }
}
