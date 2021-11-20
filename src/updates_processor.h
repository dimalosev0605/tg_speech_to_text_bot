#ifndef UPDATES_PROCESSOR_H
#define UPDATES_PROCESSOR_H

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <boost/log/trivial.hpp>

#include <thread>

#include "session.h"
#include "ini_reader.h"
#include "callbacks.h"
#include "gcloud_at_gen.h"
#include "base64.h"
#include "sample_rate_extractor.h"

class updates_processor
{
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;
    threadsafe_queue& queue_;

    std::unique_ptr<std::thread> thread_;

private:
    void process_message(boost::json::object& message);

public:
    explicit updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue);
    void run();
    ~updates_processor();
};

#endif // UPDATES_PROCESSOR_H
