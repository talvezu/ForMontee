#include "PriorityQ.h"
#include "PriorityQueueElement.h"
#include "global_settings.h"
#include "thread_safe_queue.h"
#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>


TEST(PQTests, test_size)
{
    PQueue<PriorityQueueElement> Q;
    //time_point tp = std::chrono::milliseconds(100);

    auto elem = std::make_shared<PriorityQueueElement>(
        std::vector<int>{1, 2},
        5,                                //_occurances,
        std::vector<float>({1, 2, 3, 4}), //std::vector<float> _values,
        std::vector<float>({}),
        200);

    Q.push(elem);
}


TEST(PQTests, test_occurences)
{
    //logging::active = true;
    int occurences = 10;
    PQueue<PriorityQueueElement> Q;
    shared_ptr<PQueue<PriorityQueueElement>> pElem;
    static constexpr uint64_t invokation_delta = 200;
    auto values = std::vector<float>({1, 2, 3, 4});
    auto elem =
        std::make_shared<PriorityQueueElement>(std::vector<int>{1, 2},
                                               occurences, //_occurances,
                                               values,
                                               std::vector<float>({}),
                                               invokation_delta);

    Q.push(elem);

    int elem_count = 0;
    while (occurences--)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(invokation_delta));
        auto poped_elem = Q.try_pop();
        if (poped_elem)
        {
            if (settings::logging::active)
                std::cout << *poped_elem.get();

            ASSERT_EQ(poped_elem->get_value(),
                      values[(elem_count++) % values.size()]);
        }
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(invokation_delta * 2));
    Q.try_pop();
    //Q.status_report();
    ASSERT_EQ(0, Q.get_size());
}
