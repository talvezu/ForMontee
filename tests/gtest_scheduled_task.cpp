#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include "Storage.h"
#include "PeriodicTask.h"

using namespace std::chrono_literals;

class DBEntry{
public:
    int counter;
    DBEntry()
    {
        counter = 0;
    }

    void act()
    {
        counter++;
    }
};

using storage = Storage<DBEntry>;
Storage<DBEntry> s;


void test_for(std::chrono::milliseconds duration){

    auto end_task = std::make_shared<std::atomic<bool>>(false);

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

    PeriodicTask<DBEntry> task_10(task_control_block_10);
    PeriodicTask<DBEntry> task_20(task_control_block_20);
	PeriodicTask<DBEntry> task_40(task_control_block_40);

	std::this_thread::sleep_for(duration);

	end_task->store(true);
    std::this_thread::sleep_for(10ms);
}

TEST(ScheduledTaskTest, test_basic)
{
    test_for(1000ms);
    for (const auto& elem : s.DB) {
        //if (logging::active)
            std::cout << elem.first << " -> " << elem.second.counter << '\n';
    }

    EXPECT_GE (s.DB[static_cast<uint32_t>(storage::component::engine_1)].counter, 100 - 10);
    EXPECT_GE (s.DB[static_cast<uint32_t>(storage::component::engine_2)].counter, 50 - 5);
    EXPECT_GE (s.DB[static_cast<uint32_t>(storage::component::engine_3)].counter, 25 - 2);

}
