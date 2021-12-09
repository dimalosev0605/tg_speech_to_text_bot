#include "updates_processor.h"

updates_processor::updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context,threadsafe_queue& queue, enabled_users& enabled_users)
    : io_context_{io_context},
      ssl_context_{ssl_context},
      queue_{queue},
      enabled_users_{enabled_users}
{}

void updates_processor::run()
{
    thread_ = std::make_unique<std::thread>([this]{
        if(chats_settings_.open()) {
            while(true) {
                auto obj = queue_.wait_and_pop();
                BOOST_LOG_TRIVIAL(info) << "obj: " << obj;
                if(obj.contains("stop")) {
                    break;
                } else if(obj.contains("message")) {
                    process_message(obj);
                } else if(obj.contains("action")) {
                    process_action(obj);
                } else if(obj.contains("callback_query")) {
                    process_callback_query(obj);
                }
            }
        }
    });
}

void updates_processor::process_message(boost::json::object& message_obj)
{
    auto message = message_obj["message"].as_object();

    if(message.contains("voice")) {
        process_voice_message(message);
    } else if(message.contains("entities") && message.contains("text")) {
        process_bot_command(message);
    }
}

void updates_processor::process_action(boost::json::object& action_obj)
{
    auto action = static_cast<actions>(action_obj["action"].as_int64());
    switch (action) {
    case actions::download_voice_from_tg: {
        std::string target = action_obj["target"].as_string().c_str();
        auto request = get_download_voice_from_tg_req(target);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request, download_voice_from_tg, action_obj)->run(service::telegram);
        break;
    }
    case actions::google_recognize: {
        auto voice_message_path = action_obj["voice_message_path"].as_string().c_str();
        auto chat_id = action_obj["chat_id"].as_int64();
        auto user_id = action_obj["user_id"].as_int64();
        auto request = get_google_recognize_req(voice_message_path, chat_id, user_id);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request, google_recognize, action_obj)->run(service::google);
        break;
    }
    case actions::send_voice_transcript: {
        boost::json::object query_obj;
        query_obj["chat_id"] = std::to_string(action_obj["chat_id"].as_int64());
        query_obj["reply_to_message_id"] = std::to_string(action_obj["reply_to_message_id"].as_int64());
        query_obj["text"] = action_obj["transcript"].as_string().c_str();
        auto request = get_tg_req("sendMessage", query_obj);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
        break;
    }
    }
}

void updates_processor::process_callback_query(boost::json::object& callback_query_obj)
{
    auto callback_query = callback_query_obj["callback_query"].as_object();
    auto data = callback_query["data"].as_string();
    auto message = callback_query["message"].as_object();
    auto message_id = message["message_id"].as_int64();
    auto chat = message["chat"].as_object();
    auto chat_id = chat["id"].as_int64();
    auto from = callback_query["from"].as_object();
    auto from_id = from["id"].as_int64();
    boost::json::string username;
    if(from.contains("username")) {
        username = from["username"].as_string();
    }

    chats_settings_.insert_chat_settings(chat_id, from_id, username.c_str(), data.c_str());

    boost::json::object req_params;
    req_params["chat_id"] = std::to_string(chat_id);
    req_params["message_id"] = std::to_string(message_id);
    auto request = get_tg_req("deleteMessage", req_params);
    std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
}

void updates_processor::process_voice_message(boost::json::object& voice_message_obj)
{
    auto from = voice_message_obj["from"].as_object();
    auto from_id = from["id"].as_int64();
    boost::json::string username;
    if(from.contains("username")) {
        username = from["username"].as_string();
    }
    auto chat = voice_message_obj["chat"].as_object();
    auto chat_id = chat["id"].as_int64();
    auto date = voice_message_obj["date"].as_int64();
    auto message_id = voice_message_obj["message_id"].as_int64();

    if(enabled_users_.contains(std::string{username.c_str()})) {
        auto voice =  voice_message_obj["voice"].as_object();
        auto duration = voice["duration"].as_int64();
        if(duration > 60) {
            boost::json::object query_json;
            query_json["chat_id"] = std::to_string(chat_id);
            query_json["reply_to_message_id"] = std::to_string(message_id);
            query_json["text"] = "Sorry, voice message too long. I can handle only voice messages with duration less than 60 seconds.";
            auto request = get_tg_req("sendMessage", query_json);
            std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
        } else {
            boost::json::object query_json;
            query_json["file_id"] = voice["file_id"].as_string().c_str();
            auto request = get_tg_req("getFile", query_json);
            boost::json::object chat_info;
            chat_info["chat_id"] = chat_id;
            chat_info["user_id"] = from_id;
            chat_info["reply_to_message_id"] = message_id;
            chat_info["username"] = std::string{username.c_str()} + "_" + std::to_string(from_id);
            chat_info["date"] = date;
            std::make_shared<session>(io_context_, ssl_context_, queue_, request, prepare_voice_downloading_from_tg, chat_info)->run(service::telegram);
        }
    }
}

void updates_processor::process_bot_command(boost::json::object& message_obj)
{
    auto query_json = bot_commands_processor_.process_bot_command(message_obj);
    auto request = get_tg_req("sendMessage", query_json);
    std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
}

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_tg_req(const std::string& method, boost::json::object& params)
{
    boost::beast::http::request<boost::beast::http::string_body> request;
    auto tg_req_params = ini_reader::instance().get_tg_req_params();
    request.version(tg_req_params.version_);
    request.method(boost::beast::http::verb::get);
    request.set(boost::beast::http::field::host, tg_req_params.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    std::string query_str = "/bot" + tg_req_params.token_ + "/" + method + "?";
    for(const auto& param : params) {
        std::string key = std::string{param.key_c_str()};
        std::string value = std::string{param.value().as_string().c_str()};
        query_str += key + "=" + value + "&";
    }
    bot_utility::get_correct_encoded_url_str(query_str);
    if(!query_str.empty() && query_str.back() == '&') {
        query_str.pop_back();
    }
    request.target(query_str);
    return request;
}

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_download_voice_from_tg_req(const std::string& target)
{
    boost::beast::http::request<boost::beast::http::string_body> request;
    auto tg_req_params = ini_reader::instance().get_tg_req_params();
    request.version(tg_req_params.version_);
    request.method(boost::beast::http::verb::get);
    request.set(boost::beast::http::field::host, tg_req_params.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.target(target);
    return request;
}

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_google_recognize_req(const std::string& voice_message_path, std::int64_t chat_id, std::int64_t user_id)
{
    boost::beast::http::request<boost::beast::http::string_body> request;
    auto google_req_params = ini_reader::instance().get_google_req_params();
    request.version(google_req_params.version_);
    request.method(boost::beast::http::verb::post);
    request.set(boost::beast::http::field::host, google_req_params.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(boost::beast::http::field::authorization, "Bearer " + gcloud_at_gen::instance().generate_access_token());
    request.set(boost::beast::http::field::content_type, "application/json");
    request.set(boost::beast::http::field::accept, "application/json");
    std::string target = google_req_params.method_ + "?key=" + google_req_params.key_;
    request.target(target);

    boost::json::object config_obj;
    config_obj["languageCode"] = chats_settings_.get_language_code_by_id(chat_id, user_id);
    config_obj["encoding"] = "OGG_OPUS";
    auto sample_rate = sample_rate_extractor::instanse().extract_sample_rate(voice_message_path);
    config_obj["sampleRateHertz"] = sample_rate;

    boost::json::object audio_obj;
    std::ifstream input(voice_message_path, std::ios::binary);
    std::string data = std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    std::string data_base64;
    bot_utility::base64::encode(data, &data_base64);
    audio_obj["content"] = data_base64;

    boost::json::object req_obj;
    req_obj["config"] = config_obj;
    req_obj["audio"] = audio_obj;

    request.body() = boost::json::serialize(req_obj);
    request.prepare_payload();

    return request;
}

updates_processor::~updates_processor()
{
    queue_.push_stop_obj();
    thread_->join();
}
