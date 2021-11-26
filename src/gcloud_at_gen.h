#ifndef GCLOUD_AT_GEN_H
#define GCLOUD_AT_GEN_H

#include <boost/process.hpp>
#include <chrono>
#include "ini_reader.h"

class gcloud_at_gen
{
    std::mutex m_;
    std::string access_token_;
    std::chrono::system_clock::time_point prev_gen_time_;
    const int access_token_generation_interval_;

private:
    gcloud_at_gen();

public:
    static gcloud_at_gen& instance();

    gcloud_at_gen(const gcloud_at_gen&) = delete;
    gcloud_at_gen& operator=(const gcloud_at_gen&) = delete;

    gcloud_at_gen(gcloud_at_gen&&) = delete;
    gcloud_at_gen& operator=(gcloud_at_gen&&) = delete;

    std::string generate_access_token();
};

#endif // GCLOUD_AT_GEN_H
