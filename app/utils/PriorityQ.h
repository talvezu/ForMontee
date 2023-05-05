#pragma once
#include <chrono>
#include <queue>
#include <mutex>
#include <memory>
#include <iostream>
#include "PriorityQueueElement.h"
#include "logging.h"

using std::cout;
using std::shared_ptr;
using Clock = std::chrono::steady_clock;
using time_point = std::chrono::time_point<Clock>;


template<class T>
//std::optional<time_t> optionalGetNextInvokation(T* obj)
std::shared_ptr<T> optionalNextInvokationItem(T* obj)
{
    constexpr bool has_get_rescheduled_item_if_any = requires(T* t) {
        t->get_rescheduled_item_if_any();
    };

    if constexpr (has_get_rescheduled_item_if_any)
        return obj->get_rescheduled_item_if_any();
    else
        return shared_ptr<T>();
}

template <class T>
class PQueue {
public:
    ~PQueue() {  }

    // add a new PriorityQueueElement to the queue
    template<class... Args>
    void emplace(Args&&... args) {
        std::scoped_lock lock(m_mutex);

        //m_queue.emplace(PriorityQueueElement{std::forward<Args>(args)...});
    }

    // Wait until it's time to fire the event that is first in the queue
    // which may change while we are waiting, but that'll work too.

    void status_report()
    {
        auto t = Clock::now();
        std::cout << "m_queue.size(): "<< m_queue.size() << std::endl;
        auto print_to_console = [t, this]()
        {
            // Get the current time point
            //auto now = std::chrono::steady_clock::now();

            // Convert the time point to milliseconds
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t);

            // Get the duration since the epoch
            auto duration = ms.time_since_epoch();
            auto duration_top = (std::chrono::time_point_cast<std::chrono::milliseconds>(this->m_queue.top()->next_invokation)).time_since_epoch();

            // Print the duration in milliseconds
            std::cout << duration.count() << " milliseconds since the epoch" << std::endl;
            std::cout << duration_top.count() << " top milliseconds since the epoch" << std::endl;

        };

        print_to_console();
    }

    shared_ptr<T> try_pop() {
        if (logging::active)
            status_report();
        std::unique_lock lock(m_mutex);

        auto t = Clock::now();
        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t);

        if (!m_queue.empty() && ms > m_queue.top()->next_invokation)
        {
            auto elem = m_queue.top();
            m_queue.pop();

            auto replace_elem = optionalNextInvokationItem<PriorityQueueElement>(elem.get());
            if (replace_elem)
            {
                m_queue.push(replace_elem);
            }
            return elem;
        }
        return shared_ptr<T>();
    }

    void push(shared_ptr<T> elem)
    {
        std::scoped_lock lock(m_mutex);
        m_queue.push(elem);
    }

    int get_size()
    {
        return static_cast<int>(m_queue.size());
    }

private:
    std::priority_queue<shared_ptr<PriorityQueueElement>, std::vector<shared_ptr<PriorityQueueElement>>, my_compare> m_queue;
    mutable std::mutex m_mutex;
};
