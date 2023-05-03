#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include "thread_safe_queue.h"
#include "PriorityQ.h"
#include "PriorityQueueElement.h"

TEST(PQTests, test_size)
{
    PQueue<PriorityQueueElement> Q;
    //time_point tp = std::chrono::milliseconds(100);

    auto elem = std::make_shared<PriorityQueueElement>(
                        std::vector<string>{"motor_1","motor_2"},
                        false,//_is_repeatable,
                        5,//_occurances,
                        std::vector<float>({1,2,3,4}), //std::vector<float> _values,
                        std::vector<float>({}),
                        200
                        );

    //Q.push(elem);
    ASSERT_EQ (1, Q.get_size());
}

/*
TEST(PQTests, test_occurences)
{
    int occurences = 5;
    PQueue<PriorityQueueElement> Q;
    auto elem = PriorityQueueElement(std::vector<string>{"motor_1","motor_2"},
                         false,//_is_repeatable,
                         5,//_occurances,
                         {1,2,3,4}, //std::vector<float> _values,
                         {},
                         200
                         );
    Q.push(elem);

    while(occurences--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        PriorityQueueElement elem;
        bool  = Q.pop();
    }
    ASSERT_EQ (0, Q.get_size());
}
*/
