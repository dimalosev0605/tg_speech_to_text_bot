#ifndef BOT_COMMANDS_PROCESSOR_H
#define BOT_COMMANDS_PROCESSOR_H

#include <string>
#include <vector>

#include <boost/json.hpp>

#include "bot_utility.h"
#include "ini_reader.h"

class bot_commands_processor
{
    const std::vector<std::tuple<std::string, std::string>> text_callback_data_arr_ = { {"Russian","ru-Ru"}, {"English","en-US"}, {"French","fr-FR"}, {"Spanish","es-ES"}, {"German", "de-DE"} };
    const std::string bot_name_ = ini_reader::instance().get_tg_req_params().bot_name_;

private:
    std::string get_command_text(const std::string& chat_type, const std::string& command) const;

public:
    boost::json::object process_bot_command(boost::json::object& message_obj) const;
};

#endif // BOT_COMMANDS_PROCESSOR_H
