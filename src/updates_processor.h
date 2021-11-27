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
#include "sample_rate_extractor.h"
#include "base64.h"

class updates_processor
{
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;
    threadsafe_queue& queue_;

    std::unique_ptr<std::thread> thread_;

private:
    void process_message(boost::json::object& update_obj);
    void process_action(boost::json::object& action_obj);

    boost::beast::http::request<boost::beast::http::string_body> get_send_tg_message_req(std::int64_t chat_id, std::int64_t reply_to_message_id, std::string text);
    boost::beast::http::request<boost::beast::http::string_body> get_prepare_voice_downloading_from_tg_req(const std::string& file_id);
    boost::beast::http::request<boost::beast::http::string_body> get_download_voice_from_tg_req(const std::string& target);
    boost::beast::http::request<boost::beast::http::string_body> get_google_recognize_req(const std::string& voice_message_path);

public:
    explicit updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue);
    void run();
    ~updates_processor();
};

#endif // UPDATES_PROCESSOR_H
