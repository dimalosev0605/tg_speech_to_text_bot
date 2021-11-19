#include "updates_processor.h"

updates_processor::updates_processor(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue)
    : io_context_{io_context},
      ssl_context_{ssl_context},
      queue_{queue}
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
                BOOST_LOG_TRIVIAL(info) << update_obj;
                process_message(update_obj["message"].as_object());
            } else if(update_obj.contains("action")) {
                auto action = static_cast<actions>(update_obj["action"].as_int64());
                switch (action) {
                case actions::download_voice_from_tg: {
                    boost::beast::http::request<boost::beast::http::string_body> request;
                    auto tg_req_params = ini_reader::instance().get_tg_req_params();
                    request.version(tg_req_params.version_);
                    request.method(boost::beast::http::verb::get);
                    request.set(boost::beast::http::field::host, tg_req_params.host_);
                    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                    std::string target = update_obj["target"].as_string().c_str();
                    request.target(target);
                    std::make_shared<session>(io_context_, ssl_context_, queue_, request, download_voice_from_tg, update_obj)->run(service::telegram);
                    break;
                }
                case actions::prepare_voice_downloading_from_tg: {
                    break;
                }
                case actions::google_recognize: {
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
                    config_obj["sampleRateHertz"] = 16000;

                    boost::json::object audio_obj;
                    std::ifstream input(update_obj["voice_message_path"].as_string().c_str(), std::ios::binary);
                    std::string data = std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
                    std::string data_base64;
                    base64::encode(data, &data_base64);
                    audio_obj["content"] = data_base64;

                    boost::json::object req_obj;
                    req_obj["config"] = config_obj;
                    req_obj["audio"] = audio_obj;

                    request.body() = boost::json::serialize(req_obj);
                    request.prepare_payload();

                    std::make_shared<session>(io_context_, ssl_context_, queue_, request, google_recognize, update_obj)->run(service::google);
                    break;
                }
                case actions::send_voice_transcript: {
                    boost::beast::http::request<boost::beast::http::string_body> request;
                    auto tg_req_params = ini_reader::instance().get_tg_req_params();
                    request.version(tg_req_params.version_);
                    request.method(boost::beast::http::verb::get);
                    request.set(boost::beast::http::field::host, tg_req_params.host_);
                    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                    std::string text = update_obj["transcript"].as_string().c_str();
                    auto chat_id = update_obj["chat_id"].as_int64();
                    auto reply_to_message_id = update_obj["reply_to_message_id"].as_int64();
                    std::string target = "/bot" + tg_req_params.token_ + "/sendMessage?chat_id=" + std::to_string(chat_id) + "&reply_to_message_id=" + std::to_string(reply_to_message_id) + "&text=" + text;
                    request.target(target);
                    std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
                    break;
                }
                }
            } else if(update_obj.contains("download_file")) {
                std::string target = ini_reader::instance().get_google_req_params().method_ + "?key=" + ini_reader::instance().get_google_req_params().key_;
            }
        }
    });
}

void updates_processor::process_message(boost::json::object& message)
{
    auto chat = message["chat"].as_object();
    auto id = chat["id"].as_int64();
    auto username = chat["username"].as_string();
    auto date = message["date"].as_int64();
    auto message_id = message["message_id"].as_int64();
    auto tg_req_params = ini_reader::instance().get_tg_req_params();
    boost::beast::http::request<boost::beast::http::string_body> request;

    if(message.contains("voice")) {
        auto voice =  message["voice"].as_object();
        auto duration = voice["duration"].as_int64();
        if(duration > 60) {
            request.version(tg_req_params.version_);
            request.method(boost::beast::http::verb::get);
            request.set(boost::beast::http::field::host, tg_req_params.host_);
            request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            std::string text = "sorry, voice message too long";
            std::string target = "/bot" + ini_reader::instance().get_tg_req_params().token_ + "/sendMessage?chat_id=" + std::to_string(id) + "&reply_to_message_id=" + std::to_string(message_id) + "&text=" + text;
            request.target(target);
            std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
        } else {
            request.version(tg_req_params.version_);
            request.method(boost::beast::http::verb::get);
            request.set(boost::beast::http::field::host, tg_req_params.host_);
            request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            auto file_id = voice["file_id"].as_string();
            boost::json::object chat_info;
            chat_info["chat_id"] = id;
            chat_info["reply_to_message_id"] = message_id;
            chat_info["username"] = username;
            chat_info["date"] = date;
            std::string target = "/bot" + ini_reader::instance().get_tg_req_params().token_ + "/getFile?file_id=" + std::string{file_id.c_str()};
            request.target(target);
            std::make_shared<session>(io_context_, ssl_context_, queue_, request, prepare_voice_downloading_from_tg, chat_info)->run(service::telegram);
        }
    }
    else {
        request.version(tg_req_params.version_);
        request.method(boost::beast::http::verb::get);
        request.set(boost::beast::http::field::host, tg_req_params.host_);
        request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        std::string text = "I am speech to text translator";
        std::string target = "/bot" + ini_reader::instance().get_tg_req_params().token_ + "/sendMessage?chat_id=" + std::to_string(id) + "&reply_to_message_id=" + std::to_string(message_id) + "&text=" + text;
        request.target(target);
        std::make_shared<session>(io_context_, ssl_context_, queue_, request)->run(service::telegram);
    }
}

updates_processor::~updates_processor()
{
    queue_.push_stop_obj();
    thread_->join();
}
