#pragma once
#include <chrono>
#include <queue>
#include <mutex>
#include <memory>
#include "PriorityQueueElement.h"

using std::shared_ptr;
using Clock = std::chrono::steady_clock;
using time_point = std::chrono::time_point<Clock>;

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
    shared_ptr<T> try_pop() {
        std::unique_lock lock(m_mutex);

        if (!m_queue.empty() && Clock::now() > m_queue.top()->next_invokation)
        {
            auto elem = std::move(m_queue.top());
            m_queue.pop();
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
