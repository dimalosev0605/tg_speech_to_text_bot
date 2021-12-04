#include "threadsafe_queue.h"
#include <thread>

#define BOOST_TEST_MODULE threadsafe_queue_tests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(push_single_object) {
    threadsafe_queue q;

    const int n = 100000;
    const std::string key = "v";
    const int value = 1;

    std::thread writer_t([&q, n, key, value](){
        boost::json::object obj;
        for(int i = 0; i < n; ++i) {
            obj[key] = value;
            q.push(obj);
        }
    });

    int sum = 0;
    std::thread reader_t([&q, &sum, key](){
        while(true) {
            auto obj = q.wait_and_pop();
            auto value = obj[key].as_int64();
            sum += value;
            if(sum >= n) break;
        }
    });

    reader_t.join();
    writer_t.join();

    BOOST_REQUIRE(sum == n);
}

BOOST_AUTO_TEST_CASE(push_array_of_objects) {
    threadsafe_queue q;

    const int n = 100000;
    const std::string key = "v";
    const int value = 1;
    const int arr_size = 2;

    std::thread writer_t([&q, n, key, value, arr_size](){
        boost::json::object obj;
        for(int i = 0; i < n; ++i) {
            obj[key] = value;
            boost::json::array arr(arr_size, obj);
            q.push(arr);
        }
    });

    const int check_sum = n * value * arr_size;
    int sum = 0;
    std::thread reader_t([&q, &sum, key, check_sum](){
        while(true) {
            auto obj = q.wait_and_pop();
            auto value = obj[key].as_int64();
            sum += value;
            if(sum >= check_sum) break;
        }
    });

    reader_t.join();
    writer_t.join();

    BOOST_REQUIRE(sum == check_sum);
}

BOOST_AUTO_TEST_CASE(push_stop_obj) {
    threadsafe_queue q;

    const int n = 100000;
    const int stop_number = 10000;
    const std::string key = "v";
    const int value = 1;

    std::thread writer_t([&q, n, stop_number, key, value](){
        boost::json::object obj;
        for(int i = 0; i < n; ++i) {
            if(i == stop_number) {
                q.push_stop_obj();
                continue;
            }
            obj[key] = value;
            q.push(obj);
        }
    });

    int stop_pos = 0;
    std::thread reader_t([&q, key, value, &stop_pos](){
        while(true) {
            auto obj = q.wait_and_pop();
            if(obj.contains("stop")) {
                break;
            } else {
                auto value = obj[key].as_int64();
                stop_pos += value;
            }
        }
    });

    writer_t.join();
    reader_t.join();

    BOOST_REQUIRE(stop_number == stop_pos);
}

