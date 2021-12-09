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
#include "enabled_users.h"
#include "chats_settings.h"
#include "bot_commands_processor.h"

class updates_processor
{
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;
    threadsafe_queue& queue_;
    enabled_users& enabled_users_;
    chats_settings chats_settings_;
    bot_commands_processor bot_commands_processor_;

    std::unique_ptr<std::thread> thread_;

private:
    void process_message(boost::json::object& message_obj);
    void process_action(boost::json::object& action_obj);
    void process_callback_query(boost::json::object& callback_query_obj);
    void process_voice_message(boost::json::object& voice_message_obj);
    void process_bot_command(boost::json::object& message_obj);

    boost::beast::http::request<boost::beast::http::string_body> get_tg_req(const std::string& method, boost::json::object& params);
    boost::beast::http::request<boost::beast::http::string_body> get_download_voice_from_tg_req(const std::string& target);
    boost::beast::http::request<boost::beast::http::string_body> get_google_recognize_req(const std::string& voice_message_path, std::int64_t chat_id, std::int64_t user_id);

public:
    explicit updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue, enabled_users& enabled_users);

    updates_processor(const updates_processor&) = delete;
    updates_processor& operator=(const updates_processor&) = delete;

    updates_processor(updates_processor&&) = delete;
    updates_processor& operator=(updates_processor&&) = delete;

    void run();
    ~updates_processor();
};

#endif // UPDATES_PROCESSOR_H
