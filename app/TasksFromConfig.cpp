#include <memory>
#include "TasksFromConfig.h"
#include "PeriodicTask.h"
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "NetFunctionFactory.h"
#include "InteractiveTask.h"
#include "DB.h"
#include "logging.h"


using in_out_map = map<string, map<string, shared_ptr_thread_safe_sample_queue>>;


in_out_map queues_map;

template <class T>
struct engine{
    T position;
};

engine<float>;

template <class T>
std::map<std::string, engine<T>> engines_map;
std::map<std::string, shared_ptr<InteractiveTask<uint32_t, float>>> InteractiveTasks;
/**
 * Main function for the current project
*/

float execute_net_function(std::map<std::string, std::shared_ptr<NetFunction>> net_functions, uint32_t engine, float value)
{
    if (logging::active)
        std::cout<<"callback called for engine:"<<engine<<" with value: "<<value<<"\n";

    /*to do improve, no point in searching over again*/
    for (auto net_func: net_functions)
    {
        for (auto motor: net_func.second->get_motors())
        {
            if (motor == engine)
                return net_func.second->operator()(value);
        }

    }
    return value;
}

float execute_auto_tune_function(uint32_t engine, float value)
{

    if (logging::active)
        std::cout<<"execute_net_function callback called for engine:"<<engine<<" with value: "<<value<<"\n";
    static bool increament{true};
    if (value < 0.99f && value > 0.01)
        if (increament)
            return value*=1.01f;
        else
            return value*=0.99f;
    else
    {
        if (increament)
            value-=0.02f;
        else
            value+=0.01f;

        increament = !increament;
    }
    return value;
}

void tasks_from_config_impl(workflow<float> &work_flow)
{
    auto ForMainPQ = std::make_unique<PQueue<PriorityQueueElement>>();
    work_flow.load_work();

    for (auto item: work_flow.get_tasks()){
        auto net_func = item.second;
        std::cout<<net_func->task_name<<"\n";

        std::vector<int> motors;
        for (int motor: net_func->target_motors)
        {
            motors.push_back(motor);
        }
        std::vector<float> vf{};

        auto elem = std::make_shared<PriorityQueueElement>(
            motors,
            net_func->times,
            (net_func->values.has_value())?net_func->values.value():vf,
            (net_func->delta_values.has_value())?net_func->delta_values.value():vf,
            net_func->delta_in_milliseconds);

        ForMainPQ->push(elem);
    }

    //active_net_functions
    const auto &actions = work_flow.get_net_actions();
    for (const auto &net_action: actions)
    {
        NetFunctionFactory::supportNetFunction(net_action.second.action);
    }

    const auto &action_items = work_flow.get_net_actions();
    const std::map<std::string, std::shared_ptr<NetFunction>> &net_functions = NetFunctionFactory::get_active_net_functions();

    for (auto &[net_function_name, net_action_details]: action_items)
    {
        auto net_func = net_functions.at(net_action_details.action);
        if (net_func)
        {
            net_func->set_motors(net_action_details.motors);
        }
        cout<<net_action_details.action<<"\n";
    }

    utils::DB<uint32_t, float> db;
    std::map<uint32_t, float> new_engines;
    for (auto motor_instance: work_flow.get_all_motors())
    {
        new_engines[motor_instance] = 0.5f;
    }
    db.set_component_db("Engines", new_engines);
    db.set_component_db("PositionLoop", std::map<uint32_t, float>{{1, 0.5f}});


    auto end_task = std::make_shared<std::atomic<bool>>(false);

    auto net_func_callback = std::bind(&execute_net_function, net_functions, std::placeholders::_1, std::placeholders::_2);


    /**********/
    //  main  //
    /**********/
    //won't bind, main can get db both for Engines and for PositionLoop
    auto get_db_values_callback = std::bind(&utils::DB<uint32_t,float>::get_component_db, &db, std::placeholders::_1);

    //can bind, loops does not set value
    auto engines_set_db_values_callback = std::bind(&utils::DB<uint32_t,float>::set_component_db, &db, std::string("Engines"), std::placeholders::_2);
    //auto engines_set_db_values_callback = std::bind(&utils::DB<uint32_t,float>::set_component_db, &db, std::placeholders::_1, std::placeholders::_2);
    InteractiveTasks.emplace("Main", std::make_shared<InteractiveTask<uint32_t, float>>(std::move(std::string("Main")), std::move(ForMainPQ), end_task, 100, net_func_callback, get_db_values_callback, engines_set_db_values_callback));
    InteractiveTasks.emplace("Main", std::make_shared<InteractiveTask<uint32_t, float>>(engines_set_db_values_callback));
    /**********/
    //Engines //
    /**********/
    auto engines_get_db_values_callback = std::bind(&utils::DB<uint32_t,float>::get_component_db, &db, std::string("Engines"));
    InteractiveTasks.emplace("Engines", std::make_shared<InteractiveTask<uint32_t, float>>(std::move(std::string("Engines")), std::move(ForMainPQ), end_task, 100, net_func_callback, engines_get_db_values_callback, engines_set_db_values_callback));
    /****************/
    // PositionLoop //
    /****************/
    //auto loop_set_db_values_callback = std::bind(&utils::DB<uint32_t,float>::set_component_db, &db, std::placeholders::_1, std::string("position_loop"));
    //InteractiveTasks.emplace("PositionLoop", std::make_shared<InteractiveTask<uint32_t, float>>(std::move(std::string("Engines")), std::move(ForMainPQ), end_task, 100, net_func_callback, engines_get_db_values_callback, engines_set_db_values_callback));


    std::this_thread::sleep_for(500000ms);

    end_task->store(true);


}
