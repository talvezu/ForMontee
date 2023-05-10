#include <memory>
#include "TasksFromConfig.h"
#include "PeriodicTask.h"
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "NetFunctionFactory.h"
#include "InteractiveTask.h"
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
std::map<std::string, shared_ptr<InteractiveTask>> InteractiveTasks;
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

    std::vector<std::string> routines{"Main", "Motors", "Position"};
    for (auto routine: routines)
    {
        queues_map.emplace(routine,
                           std::map<std::string, shared_ptr_thread_safe_sample_queue>
                           {{"in",  std::make_shared<thread_safe_sample_queue>()},
                            {"out", std::make_shared<thread_safe_sample_queue>()}});

    }

    const auto &action_items = work_flow.get_net_actions();
    const std::map<std::string, std::shared_ptr<NetFunction>> &net_functions = NetFunctionFactory::get_active_net_functions();
    //net_functions["roll"]->set_motors();
    for (auto &[net_function_name, net_action_details]: action_items)
    {
        auto net_func = net_functions.at(net_action_details.action);
        if (net_func)
        {
            net_func->set_motors(net_action_details.motors);
        }
        cout<<net_action_details.action<<"\n";
    }




    auto end_task = std::make_shared<std::atomic<bool>>(false);



    auto net_func_callback = std::bind(&execute_net_function, net_functions, std::placeholders::_1, std::placeholders::_2);
    InteractiveTasks.emplace("Main", std::make_shared<InteractiveTask>(std::move(std::string("Main")),queues_map["Main"]["in"], queues_map["Motors"]["in"], std::move(ForMainPQ), end_task, 100, net_func_callback));

    //InteractiveTasks.emplace("Main", std::make_shared<InteractiveTask>(std::move(std::string("Main")),queues_map["Main"]["in"], queues_map["Motors"]["in"], std::move(nullptr), end_task, 100, net_func_callback));
    InteractiveTasks.at("Main")->inject_elements_to_queue(work_flow.get_net_func_motors());
    InteractiveTasks.emplace("Motor", std::make_shared<InteractiveTask>(std::move(std::string("Motor")), queues_map["Motors"]["in"], queues_map["Main"]["in"], nullptr, end_task, 10, execute_auto_tune_function));

    std::this_thread::sleep_for(500000ms);
    end_task->store(true);

}
