#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

#include <boost/json.hpp>

class threadsafe_queue
{
    std::mutex m_;
    std::queue<boost::json::object> q_;
    std::condition_variable cv_;

public:
    threadsafe_queue() = default;

    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    threadsafe_queue(threadsafe_queue&&) = delete;
    threadsafe_queue& operator=(threadsafe_queue&&) = delete;

    void push(const boost::json::array& arr);
    void push(const boost::json::object& obj);
    boost::json::object wait_and_pop();
    void push_stop_obj();
};

#endif // THREADSAFE_QUEUE_H
