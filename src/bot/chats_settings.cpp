#include "chats_settings.h"

chats_settings::chats_settings()
{
}

bool chats_settings::open()
{
    int rc = sqlite3_open("chats_settings.sqlite3", &db_);
    char* err_msg = nullptr;
    if(rc != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << sqlite3_errmsg(db_);
        return false;
    } else {
        BOOST_LOG_TRIVIAL(info) << "Database was opened";
        const std::string qry = "create table if not exists chats_settings (chat_id integer not null, user_id integer not null, username text, language_code text, primary key (chat_id, user_id))";
        rc = sqlite3_exec(db_, qry.c_str(), nullptr, nullptr, &err_msg);
        if(rc != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << sqlite3_errmsg(db_);
            return false;
        } else {
            BOOST_LOG_TRIVIAL(info) << "Database ready";
            return true;
        }
    }
}

chats_settings::~chats_settings()
{

}

int get_language_code_by_id_callback(void* data, int col_count, char** row_values, [[maybe_unused]]char** col_names)
{
    std::string* language_code = static_cast<std::string*>(data);
    for(int i = 0; i < col_count; i++) {
        *language_code = row_values[i];
    }
    return 0;
}

std::string chats_settings::get_language_code_by_id(std::int64_t chat_id, std::int64_t user_id) const
{
    std::string language_code = "ru-RU";
    auto format = boost::format("select language_code from chats_settings where chat_id = %1% and user_id = %2%") % chat_id % user_id;
    const auto qry = format.str();

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, qry.c_str(), get_language_code_by_id_callback, &language_code, &err_msg);
    if(rc != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << sqlite3_errmsg(db_);
    }

    return language_code;
}

bool chats_settings::insert_chat_settings(std::int64_t chat_id, std::int64_t user_id, const std::string& username,const std::string& language_code)
{
    auto format = boost::format("insert into chats_settings (chat_id, user_id, username, language_code) values (%1%, %2%,'%3%', '%4%') "
                                "on conflict (chat_id, user_id) do update set username = excluded.username, language_code = excluded.language_code")
                                % chat_id % user_id % username % language_code;
    const auto qry = format.str();
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, qry.c_str(), nullptr, nullptr, &err_msg);
    if(rc != SQLITE_OK) {
        BOOST_LOG_TRIVIAL(error) << sqlite3_errmsg(db_);
        return false;
    }
    return true;
}
