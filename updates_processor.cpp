#include "updates_processor.h"

updates_processor::updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue)
    : io_context_{io_context},
      ssl_context_{ssl_context},
      queue_{queue}
{}

void updates_processor::run()
{
    io_context_thread_ = std::make_unique<std::thread>([this]{
        io_context_.run();
    });
    queue_extractor_thread_ = std::make_unique<std::thread>([this]{
        while(true) {
            auto update_obj = queue_.wait_and_pop();
            BOOST_LOG_TRIVIAL(info) << "update obj: " << update_obj;
            if(update_obj.contains("message")) {
                process_message(update_obj["message"].as_object());
            }
        }
    });
}

void updates_processor::process_message(boost::json::object& message)
{
    if(message.contains("text")) {
        auto chat = message["chat"].as_object();
        auto id = chat["id"].as_int64();
        auto text = message["text"].as_string().c_str();
        std::string method = "sendMessage?chat_id=" + std::to_string(id) + "&text=" + text;
        std::make_shared<session>(io_context_, ssl_context_, ini_reader::instance().get_request_settings(), method)->run();
    }
}
