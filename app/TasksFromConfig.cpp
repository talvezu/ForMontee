#include <memory>
#include "TasksFromConfig.h"
#include "PeriodicTask.h"
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "NetFunctionFactory.h"
#include "InteractiveTask.h"
#include "logging.h"



PQueue<PriorityQueueElement> ForEnginesPQ;
PQueue<PriorityQueueElement> ForMainPQ;

template <class T>
struct engine{
    T position;
};

engine<float>;

template <class T>
std::map<std::string, engine<T>> engines_map;

/**
 * Main function for the current project
*/
void tasks_from_config_impl(workflow<float> &work_flow)
{
    work_flow.load_work();
    for (auto item: work_flow.get_tasks()){
        auto net_func = item.second;
        std::cout<<net_func->task_name<<"\n";

        std::vector<string> motors_name;
        for (int motor: net_func->target_motors)
        {
            motors_name.push_back("motor_"+std::to_string(motor));
        }
        std::vector<float> vf{};

        auto elem = std::make_shared<PriorityQueueElement>(
            motors_name,
            net_func->times,
            (net_func->values.has_value())?net_func->values.value():vf,
            (net_func->delta_values.has_value())?net_func->delta_values.value():vf,
            net_func->delta_in_milliseconds);

        ForEnginesPQ.push(elem);
    }

    //active_net_functions
    const auto &actions = work_flow.get_net_actions();
    for (const auto net_action: actions )
    {
        NetFunctionFactory::supportNetFunction(net_action.second.action);
    }

}
