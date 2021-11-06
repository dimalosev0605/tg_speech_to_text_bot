#include "threadsafe_queue.h"

void threadsafe_queue::push(const boost::json::array& arr)
{
    std::lock_guard<std::mutex> lock(m_);
    for(const auto& elem : arr) {
        auto obj = elem.as_object();
        q_.push(std::move(obj));
    }
    cv_.notify_all();
}

boost::json::object threadsafe_queue::wait_and_pop()
{
    std::unique_lock<std::mutex> lock(m_);
    cv_.wait(lock, [this] { return !q_.empty(); });
    auto obj = q_.front();
    q_.pop();
    return obj;
}
