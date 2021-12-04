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
        while(true) {
            auto update_obj = queue_.wait_and_pop();
            if(update_obj.contains("stop")) {
                if(update_obj["stop"].as_bool()){
                    queue_.push_stop_obj();
                    break;
                }
            }
            if(update_obj.contains("message")) {
                BOOST_LOG_TRIVIAL(info) << "update_obj:\n" << update_obj;
                process_message(update_obj);
            } else if(update_obj.contains("action")) {
                process_action(update_obj);
            }
        }
    });
}

void updates_processor::process_message(boost::json::object& update_obj)
{
    auto message = update_obj["message"].as_object();
    auto chat = message["chat"].as_object();
    auto id = chat["id"].as_int64();
    auto username = chat["username"].as_string();
    auto date = message["date"].as_int64();
    auto message_id = message["message_id"].as_int64();

    if(enabled_users_.contains(std::string{username.c_str()})) {
        if(message.contains("voice")) {
            auto voice =  message["voice"].as_object();
            auto duration = voice["duration"].as_int64();
            if(duration > 60) {
                auto request = get_send_tg_message_req(id, message_id, "Sorry, voice message too long. I can handle only voice messages with duration less than 60 seconds.");
                std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
            } else {
                auto file_id = voice["file_id"].as_string().c_str();
                auto request = get_prepare_voice_downloading_from_tg_req(file_id);
                boost::json::object chat_info;
                chat_info["chat_id"] = id;
                chat_info["reply_to_message_id"] = message_id;
                chat_info["username"] = username;
                chat_info["date"] = date;
                std::make_shared<session>(io_context_, ssl_context_, queue_, request, prepare_voice_downloading_from_tg, chat_info)->run(service::telegram);
            }
        } else {
            auto request = get_send_tg_message_req(id, message_id, "I am speech to text translator. I can handle only voice messages.");
            std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
        }
    } else {
        auto request = get_send_tg_message_req(id, message_id, "Ti kto?");
        std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
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
        auto request = get_google_recognize_req(voice_message_path);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request, google_recognize, action_obj)->run(service::google);
        break;
    }
    case actions::send_voice_transcript: {
        std::string text = action_obj["transcript"].as_string().c_str();
        auto chat_id = action_obj["chat_id"].as_int64();
        auto reply_to_message_id = action_obj["reply_to_message_id"].as_int64();
        auto request = get_send_tg_message_req(chat_id, reply_to_message_id, text);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
        break;
    }
    }
}

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_send_tg_message_req(std::int64_t chat_id, std::int64_t reply_to_message_id, std::string text)
{
    boost::beast::http::request<boost::beast::http::string_body> request;
    auto tg_req_params = ini_reader::instance().get_tg_req_params();
    request.version(tg_req_params.version_);
    request.method(boost::beast::http::verb::get);
    request.set(boost::beast::http::field::host, tg_req_params.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    std::replace(text.begin(), text.end(), ' ', '+');
    std::string target = "/bot" + tg_req_params.token_ + "/sendMessage?chat_id=" + std::to_string(chat_id) + "&reply_to_message_id=" + std::to_string(reply_to_message_id) + "&text=" + text;
    request.target(target);
    return request;
}

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_prepare_voice_downloading_from_tg_req(const std::string& file_id)
{
    boost::beast::http::request<boost::beast::http::string_body> request;
    auto tg_req_params = ini_reader::instance().get_tg_req_params();
    request.version(tg_req_params.version_);
    request.method(boost::beast::http::verb::get);
    request.set(boost::beast::http::field::host, tg_req_params.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    std::string target = "/bot" + tg_req_params.token_ + "/getFile?file_id=" + std::string{file_id.c_str()};
    request.target(target);
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

boost::beast::http::request<boost::beast::http::string_body> updates_processor::get_google_recognize_req(const std::string& voice_message_path)
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
    config_obj["languageCode"] = "ru-RU";
    config_obj["encoding"] = "OGG_OPUS";
    auto sample_rate = sample_rate_extractor::instanse().extract_sample_rate(voice_message_path);
    config_obj["sampleRateHertz"] = sample_rate;

    boost::json::object audio_obj;
    std::ifstream input(voice_message_path, std::ios::binary);
    std::string data = std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    std::string data_base64;
    base64::encode(data, &data_base64);
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
