#pragma once
#pragma once
#include <iostream>
#include <optional>
#include <thread>
#include <string>
#include <functional>
#include <limits>
#include "debug.h"

using std::cout;
using std::nullopt;
using std::thread;
using namespace std::chrono_literals;

template<class T>
class TaskControlBlock
{
public:
	TaskControlBlock(std::atomic<bool>& _end_task) :end_task(_end_task), milli_interval(INT_MAX), task_name("unnamed_task") {
		process = nullptr;
		readData = nullptr;
		writeData = nullptr;
	};
	std::atomic<bool>& end_task;
	int milli_interval;
	std::string task_name;
	unsigned mask;
	std::function<void(std::shared_ptr<T> &Entry)> process;
	std::function<void(std::shared_ptr<T> &Entry)> readData;
	std::function<void(std::shared_ptr<T> &Entry, unsigned mask)> writeData;

	TaskControlBlock<T>& operator=(TaskControlBlock<T>& rhs)
	{
		end_task = rhs.end_task;
		milli_interval = rhs.milli_interval;
		process = rhs.process;
		readData = rhs.readData;
		writeData = rhs.writeData;
	}
};

template<class T>
class PeriodicTask
{
	std::thread task_context;
	TaskControlBlock<T> task_control_block;

	void thread_routine(TaskControlBlock<T> &task_control_block) {
		thread_local unsigned sample_id = 0;
		cout << std::this_thread::get_id() << " interval " << task_control_block.milli_interval << "\n";
		auto sleep_period = std::chrono::duration<int, std::milli>(task_control_block.milli_interval);
		std::shared_ptr<T> entry = std::make_shared<T>();

		while (!task_control_block.end_task.load())
		{
			std::this_thread::sleep_for(sleep_period);
			//cout << std::this_thread::get_id() << " " << sample_id << "\n";
			if (nullptr != task_control_block.readData)
				task_control_block.readData(entry);
			if (nullptr != task_control_block.process)
				task_control_block.process(entry);
			if (nullptr != task_control_block.writeData)
				task_control_block.writeData(entry, task_control_block.mask);

			if (debugging::active)
			{
				std::cout << task_control_block.task_name << " updated entry mask " << task_control_block.mask << "\n";
			}
		}
		cout << std::this_thread::get_id() << " Done\n";
	};


public:
	PeriodicTask(TaskControlBlock<T> _task_control_block) :task_control_block(_task_control_block), task_context{} {
		task_context = std::thread(&PeriodicTask::thread_routine, this, std::ref(task_control_block));
	};


	~PeriodicTask() {
		task_context.join();
		std::cout << task_control_block.task_name<< "joined\n";
	};
};

