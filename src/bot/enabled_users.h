#ifndef ENABLED_USERS_H
#define ENABLED_USERS_H

#include <unordered_set>
#include <fstream>
#include <chrono>
#include <mutex>

#include <boost/json.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>

#include "ini_reader.h"

class enabled_users
{
    std::unordered_set<std::string> enabled_users_;
    std::mutex m_;
    std::chrono::system_clock::time_point prev_enabled_users_read_time_;
    const int enabled_users_read_interval_;
    const std::string enabled_users_file_path_;
    bool use_enabled_users_;

private:
    boost::json::value read_file() const;
    void fill_enabled_users();

public:
    explicit enabled_users(const std::string& enabled_users_file_path = "enabled_users.json");

    enabled_users(const enabled_users&) = delete;
    enabled_users& operator=(const enabled_users&) = delete;

    enabled_users(enabled_users&&) = delete;
    enabled_users& operator=(enabled_users&&) = delete;

    bool contains(std::string user);
};

#endif // ENABLED_USERS_H
