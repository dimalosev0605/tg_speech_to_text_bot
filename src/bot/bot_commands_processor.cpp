#include "bot_commands_processor.h"

std::string bot_commands_processor::get_command_text(const std::string& chat_type, const std::string& command) const
{
    std::string command_text;
    if(chat_type == "group" || chat_type == "supergroup") {
        command_text = "/" + command + "@" + bot_name_;
    } else if (chat_type == "private") {
        command_text = "/" + command;
    } else {
        return command;
    }
    return command_text;
}

boost::json::object bot_commands_processor::process_bot_command(boost::json::object& message_obj) const
{
    auto chat = message_obj["chat"].as_object();
    auto chat_id = chat["id"].as_int64();
    auto message_id = message_obj["message_id"].as_int64();
    auto chat_type = chat["type"].as_string();

    auto entities = message_obj["entities"].as_array();
    if(!entities.empty()) {
        auto entity_obj = entities[0].as_object();
        auto entity_type = entity_obj["type"].as_string();
        if(std::string{entity_type.c_str()} == "bot_command") {
            auto text = message_obj["text"].as_string();
            if(get_command_text(chat_type.c_str(), "language") == std::string{text.c_str()}) {
                boost::json::object inline_keyboard_markup;
                const int count_of_items_in_row = 2;
                const int count_of_bts = text_callback_data_arr_.size();

                int counter = 0;
                boost::json::array buttons_arr;
                boost::json::array btns_row;
                while(counter != count_of_bts) {
                    boost::json::object btn;
                    btn["text"] = std::get<0>(text_callback_data_arr_[counter]);
                    btn["callback_data"] = std::get<1>(text_callback_data_arr_[counter]);
                    btns_row.push_back(btn);
                    ++counter;
                    if(counter % count_of_items_in_row == 0) {
                        buttons_arr.push_back(btns_row);
                        btns_row.clear();
                    }
                }
                if(!btns_row.empty()) {
                    buttons_arr.push_back(btns_row);
                }
                inline_keyboard_markup["inline_keyboard"] = buttons_arr;

                boost::json::object query_json;
                query_json["chat_id"] = std::to_string(chat_id);
                query_json["message_id"] = std::to_string(message_id);
                query_json["text"] = "Please select the language of speech recognition";
                query_json["reply_markup"] = boost::json::serialize(inline_keyboard_markup);
                return query_json;

            } else if(get_command_text(chat_type.c_str(), "help") == text.c_str()) {
                boost::json::object query_json;
                query_json["chat_id"] = std::to_string(chat_id);
                query_json["reply_to_message_id"] = std::to_string(message_id);
                std::string str = "Zvukozavr recognizes speech from any voice messages that it gets from private and group chats. It use Google Speech engine.\n\n"
                                  "/help -- Shows this message\n"
                                  "/language -- Lets you pick a voice recognition language\n";
                bot_utility::get_correct_encoded_url_str(str);
                query_json["text"] = str;
                return query_json;
            }
        }
    }
    return {};
}
