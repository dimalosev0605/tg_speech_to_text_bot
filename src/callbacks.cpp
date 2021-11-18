#include "callbacks.h"

void prepare_voice_downloading_from_tg(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)
{
    boost::beast::error_code parse_ec;
    auto response_obj = boost::json::parse(response.body(), parse_ec).as_object();
    if(!parse_ec) {
        auto ok = response_obj["ok"].as_bool();
        if(ok) {
            auto result = response_obj["result"].as_object();
            if(result.contains("file_path")) {
                auto file_path = result["file_path"].as_string();
                auto file_id = result["file_id"].as_string();
                boost::json::object obj = chat_info;
                obj["action"] = static_cast<int>(actions::download_voice_from_tg);
                obj["target"] = "/file/bot" + ini_reader::instance().get_tg_req_params().token_ + "/" + std::string{file_path.c_str()};
                obj["file_id"] = file_id;
                queue.push(obj);
            }
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << parse_ec.message() << std::endl;
        return;
    }
}

void download_voice_from_tg(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)
{
    auto file_data = response.body();
    auto user_name = chat_info["username"].as_string().c_str();
    auto date = chat_info["date"].as_int64();

    std::time_t temp = date;
    std::tm* t = std::gmtime(&temp);
    std::stringstream ss;
    ss << std::put_time(t, "%Y_%m_%d_%H:%M:%S");
    std::string date_str = ss.str();

    auto file_path = ini_reader::instance().get_configuration().voice_messages_path_ + user_name + "_" + date_str + ".ogg";
    std::ofstream os(file_path, std::ios::binary);
    os << file_data;
    chat_info["action"] = static_cast<int>(actions::google_recognize);
    chat_info["voice_message_path"] = file_path;
    queue.push(chat_info);
}

void google_recognize(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)
{
    auto body = response.body();
    BOOST_LOG_TRIVIAL(info) << "body =\n" << body;
    BOOST_LOG_TRIVIAL(info) << "chat_info =\n" << chat_info;
    std::string transcript;
    auto body_obj = boost::json::parse(body).as_object();
    if(body_obj.contains("results")) {
        auto results = body_obj["results"].as_array();
        if(!results.empty()) {
            auto speech_recognition_result = results.front().as_object();
            auto alternatives = speech_recognition_result["alternatives"].as_array();
            if(!alternatives.empty()) {
                auto speech_recognition_alternative = alternatives.front().as_object();
                transcript = speech_recognition_alternative["transcript"].as_string().c_str();
            }
        }
    }

    if(transcript.empty()) {
        transcript = "not recognized";
    }
    chat_info["transcript"] = transcript;
    chat_info["action"] = static_cast<int>(actions::send_voice_transcript);
    queue.push(chat_info);
}

