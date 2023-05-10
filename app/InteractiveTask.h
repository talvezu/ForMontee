#pragma once

#include <mutex>
#include <unordered_map>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "thread_safe_queue.h"
#include "logging.h"


struct sample_data{
    int motor_id;
    float value;
    sample_data(uint32_t id, float v) : motor_id(id), value(v) {}
};

using std::string;
using std::map;
using thread_safe_sample_queue = threadsafe_queue<sample_data>;
using shared_ptr_thread_safe_sample_queue = std::shared_ptr<threadsafe_queue<sample_data>>;

//using queue_map = std::map<std::string, std::pair<threadsafe_queue<sample_data>, threadsafe_queue<sample_data>>>;
//template<class T> to do
class InteractiveTask{
    std::string task_name;
    shared_ptr_thread_safe_sample_queue &in;
    shared_ptr_thread_safe_sample_queue &out;
    std::unique_ptr<PQueue<PriorityQueueElement>> inner_queue;
    std::shared_ptr<std::atomic<bool>> end_task;
    int milli_interval;

    std::shared_ptr<TaskControlBlock<float>> task_control_block;
    std::shared_ptr<PeriodicTask<float>> periodic_task;

    std::function<float(uint32_t, float)> callback;
public:

    InteractiveTask(std::string &&_task_name,
                    shared_ptr_thread_safe_sample_queue &_in,
                    shared_ptr_thread_safe_sample_queue &_out,
                    std::unique_ptr<PQueue<PriorityQueueElement>> _inner_queue,
                    std::shared_ptr<std::atomic<bool>> _end_task,
                    int _milli_interval,
                    std::function<float(uint32_t, float)> _callback)
                    : task_name(_task_name), in(_in), out(_out),
                    inner_queue(std::move(_inner_queue)), end_task(_end_task), milli_interval(_milli_interval),
                    task_control_block(std::make_shared<TaskControlBlock<float>>(
                                       end_task,
                                       milli_interval,
                                       task_name + "_" + std::to_string(milli_interval) + "_ms",
                                       0,
                                       std::bind(&InteractiveTask::process, this, std::placeholders::_1),
                                       std::bind(&InteractiveTask::read, this, std::placeholders::_1),
                                       std::bind(&InteractiveTask::write, this, std::placeholders::_1))),
                    periodic_task(std::make_shared<PeriodicTask<float>>(task_control_block)),
                    callback(_callback)
    {

    }

	void read(std::shared_ptr<float> &Entry)
    {
        //read pq which is net instruction for engine.
        std::set<int> motors_from_inner_queue_value;

        //reading inner queue if value from it ignore value from
        if (inner_queue) while( true )
        {
            auto elem = inner_queue->try_pop();
            if (!elem)
                break;

            float value = elem->get_value();
            for (auto motor: elem->target_motors)
            {
                motors_from_inner_queue_value.insert(motor);
                //if (logging::active)
                    std::cout<<*elem<<"\n";
                for (auto &motor: elem->target_motors)
                {
                    out->push({static_cast<uint32_t>(motor), value});
                }

            }
        }

        //read from input queue
        while( true )
        {
            auto elem = in->try_pop();
            if (!elem)
                break;

            float value = elem->value;
            if (motors_from_inner_queue_value.find(elem->motor_id) == motors_from_inner_queue_value.end())
            {
                if (logging::active)
                    std::cout<<"motor: "<<elem->motor_id<<" value:"<<value<<"\n";
                float val_returned = value;
                if (callback)
                    val_returned = callback(elem->motor_id, value);

                std::cout<<"motor: "<<elem->motor_id<<" value retured:"<<val_returned<<"\n";
                out->push({static_cast<uint32_t>(elem->motor_id), val_returned});
            }
            else
            {
                //if (logging::active)
                    std::cout<<"skipped! motor: "<<elem->motor_id<<" value:"<<value<<"\n";

            }
        }

    }

	void process(std::shared_ptr<float> &Entry)
    {

    }

    void write(std::shared_ptr<float> &Entry)
    {
        //optional write to future db;
    }

    void inject_elements_to_queue(const std::set<uint32_t> motors)
    {
        for (auto motor: motors)
        {
            in->push({motor, 0.5f});
        }
    }
};

