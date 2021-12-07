#include "enabled_users.h"

boost::json::value enabled_users::read_file() const
{
    std::ifstream is(enabled_users_file_path_);
    boost::json::stream_parser p;
    std::string line;
    while(std::getline(is, line)) {
        p.write(line);
    }
    p.finish();
    return p.release();
}

void enabled_users::fill_enabled_users()
{
    auto json = read_file().as_object();
    use_enabled_users_ = json["use_enabled_users"].as_bool();
    if(use_enabled_users_) {
        enabled_users_.clear();
        auto users = json["users"].as_array();
        for(const auto& elem : users) {
            auto user = elem.as_object();
            auto username = std::string {user["username"].as_string().c_str()};
            boost::algorithm::to_lower(username);
            enabled_users_.insert(username);
        }
    }
}

enabled_users::enabled_users(const std::string& enabled_users_file_path)
    : enabled_users_read_interval_{ini_reader::instance().get_configuration().enabled_users_read_interval_},
      enabled_users_file_path_{enabled_users_file_path}
{
    fill_enabled_users();
}

bool enabled_users::contains(std::string user)
{
    boost::algorithm::to_lower(user);
    std::lock_guard<std::mutex> lock(m_);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - prev_enabled_users_read_time_;
    if(elapsed_seconds.count() > enabled_users_read_interval_) {
        prev_enabled_users_read_time_ = now;
        fill_enabled_users();
    }

    if(use_enabled_users_) {
        return enabled_users_.contains(user);
    } else {
        return true;
    }
}
