#ifndef CHATS_SETTINGS_H
#define CHATS_SETTINGS_H

#include <sqlite3.h>

#include <unordered_map>
#include <string>

#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

class chats_settings
{
    sqlite3* db_;

public:
    explicit chats_settings();
    bool open();
    std::string get_language_code_by_id(std::int64_t id, std::int64_t user_id) const;
    bool insert_chat_settings(std::int64_t chat_id, std::int64_t user_id, const std::string& username,const std::string& language_code);
    ~chats_settings();
};

int get_language_code_by_id_callback(void* v, int argc, char **argv, char **azColName);

#endif // CHATS_SETTINGS_H
