#pragma once
#pragma once
#include "Storage.h"
#include "ThreadUtil.h"
#include "global_settings.h"
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <pthread.h>
#include <string>
#include <thread>

using std::cout;
using std::nullopt;
using std::thread;
using namespace std::chrono_literals;

template <class T>
class TaskControlBlock
{
public:
    std::shared_ptr<std::atomic<bool>> end_task;
    int milli_interval;
    std::string task_name;
    unsigned mask;
    std::function<void(std::shared_ptr<T> &Entry)> process;
    std::function<void(std::shared_ptr<T> &Entry)> readData;
    std::function<void(std::shared_ptr<T> &Entry)> writeData;

    TaskControlBlock(std::shared_ptr<std::atomic<bool>> _end_task)
        : end_task(_end_task), milli_interval(INT_MAX),
          task_name("unnamed_task")
    {
        process = nullptr;
        readData = nullptr;
        writeData = nullptr;
    };

    TaskControlBlock(std::shared_ptr<std::atomic<bool>> _end_task,
                     int _milli_interval,
                     std::string _task_name,
                     unsigned _mask,
                     std::function<void(std::shared_ptr<T> &Entry)> _process,
                     std::function<void(std::shared_ptr<T> &Entry)> _readData,
                     std::function<void(std::shared_ptr<T> &Entry)> _writeData)
        : end_task(_end_task), milli_interval(_milli_interval),
          task_name(_task_name), process(_process), readData(_readData),
          writeData(_writeData){};
    TaskControlBlock(const TaskControlBlock<T> &rhs) = default;
    TaskControlBlock<T> &operator=(const TaskControlBlock<T> &rhs) = default;

    virtual ~TaskControlBlock()
    {
        if (settings::logging::active)
            cout << "virutal ~TaskControlBlock()\n";
    }
};

template <class T>
class PeriodicTask
{
    std::thread task_context;
    bool done;
    std::string task_name;
    std::shared_ptr<TaskControlBlock<T>> task_control_block;
    void thread_routine(std::shared_ptr<TaskControlBlock<T>> task_control_block)
    {

        cout << std::this_thread::get_id() << " interval "
             << task_control_block->milli_interval << "\n";
        auto sleep_period = std::chrono::duration<int, std::milli>(
            task_control_block->milli_interval);
        std::shared_ptr<T> entry = std::make_shared<T>();

        while (!task_control_block->end_task->load())
        {
            std::this_thread::sleep_for(sleep_period);
            if (nullptr != task_control_block->readData)
            {
                task_control_block->readData(entry);
            }
            if (nullptr != task_control_block->process)
            {
                task_control_block->process(entry);
            }
            if (nullptr != task_control_block->writeData)
            {
                task_control_block->writeData(entry);
            }
            if (settings::logging::active)
            {
                //std::cout << task_control_block->task_name << " updated entry mask " << task_control_block.mask << "\n";
            }
        }
        cout << std::this_thread::get_id() << " Done\n";
        done = true;
    };

    void thread_routine_wrapper(
        std::shared_ptr<TaskControlBlock<T>> task_control_block_)
    {
        //cout<<task_control_block->task_name<<"\n";
        task_control_block = task_control_block_;
        set_thread_name(task_control_block->task_name);
        thread_routine(task_control_block);
    };


public:
    PeriodicTask(std::shared_ptr<TaskControlBlock<T>> _task_control_block)
        : task_context{std::thread(&PeriodicTask::thread_routine_wrapper,
                                   this,
                                   _task_control_block)},
          done{false}
    {
        task_name = _task_control_block->task_name;
    };

    void join()
    {
        while (!done)
            ;
    }

    ~PeriodicTask()
    {
        task_context.join();
        if (settings::logging::active)
        {
            std::cout << task_name << ": joined\n";
        }
    }
};
