#pragma once

#include <mutex>
#include <unordered_map>
#include <memory>
#include <map>
#include <iostream>
#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "thread_safe_queue.h"
#include "logging.h"


struct sample_data{
    uint32_t engine_id;
    float value;
};

using queue_map = std::map<std::string, std::pair<threadsafe_queue<sample_data>, threadsafe_queue<sample_data>>>;
class InteractiveTask{
    std::shared_ptr<queue_map> qm;
    string in_task;
    string out_task;
    PQueue<PriorityQueueElement> inner_queue;
public:

    InteractiveTask( std::shared_ptr<queue_map> _qm,
                     string _in_task,
                     string _out_task):qm(_qm), in_task(_in_task), out_task(_out_task)
    {

    }

	float read()
    {
        //read pq which is net instruction for engine.
        /*
        while(auto elem = inner_queue.try_pop(); elem != nullptr)
        {
            for (auto engine_id; elem->target_motors)
                output_queue.push({engine_id, elem->get_value()});
        }
        */

    }

	void process()
    {
        //calc from data_for_engine
        //send to engine thread;
        //while(auto elem = output_queue.try_pop(); elem != nullptr)
        //{

        //}
    }

    void write(float f)
    {
        //optional write to future db;
    }

    /*
    auto task_control_block_10 = std::make_shared<TaskControlBlock<DBEntry>>(end_task);
	task_control_block_10->milli_interval = 10;
	task_control_block_10->task_name = std::string("task_10");
	task_control_block_10->mask = static_cast<unsigned>(storage::component::engine_1);
	task_control_block_10->readData = std::bind(&storage::read_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_1));
	task_control_block_10->writeData = std::bind(&storage::write_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_1));
    task_control_block_10->process = std::bind(&storage::process, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_1));

    auto task_control_block_20 = std::make_shared<TaskControlBlock<DBEntry>>(end_task);
	task_control_block_20->milli_interval = 20;
	task_control_block_20->task_name = std::string("task_20");
	task_control_block_20->mask = static_cast<unsigned>(storage::component::engine_2);
	task_control_block_20->readData = std::bind(&storage::read_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_2));
	task_control_block_20->writeData = std::bind(&storage::write_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_2));
    task_control_block_20->process = std::bind(&storage::process, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_2));

    auto task_control_block_40 = std::make_shared<TaskControlBlock<DBEntry>>(end_task);
	task_control_block_40->milli_interval = 40;
	task_control_block_40->task_name = std::string("task_40");
	task_control_block_40->mask = static_cast<unsigned>(storage::component::engine_3);
	task_control_block_40->readData = std::bind(&storage::read_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_3));
	task_control_block_40->writeData = std::bind(&storage::write_entry, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_3));
    task_control_block_40->process = std::bind(&storage::process, &s, std::placeholders::_1, static_cast<uint32_t>(storage::component::engine_3));

    shared_ptrrPeriodicTask<DBEntry> task_10(task_control_block_10);
    PeriodicTask<DBEntry> task_20(task_control_block_20);
	PeriodicTask<DBEntry> task_40(task_control_block_40);
    */
};

