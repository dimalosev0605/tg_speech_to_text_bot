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
    // implement or delete ctors
    void push(const boost::json::array& arr);
    boost::json::object wait_and_pop();
    void push_stop_obj();
};

#endif // THREADSAFE_QUEUE_H
