#ifndef SETTINGS_STRUCTS_H
#define SETTINGS_STRUCTS_H

#include <string>
#include <iostream>

struct tg_req_params
{
    std::string host_;
    std::string port_;
    int version_;
    std::string token_;
};

struct google_req_params
{
    std::string host_;
    std::string port_;
    int version_;
    std::string method_;
    std::string key_;
    std::string service_account_key_path_;
    int access_token_generation_interval_;
};

struct config
{
    std::string voice_messages_path_;
    int processing_threads_count_;
    int io_context_threads_count_;
    std::string ca_certificates_file_;
};

std::ostream& operator<<(std::ostream& os, const tg_req_params& obj);
std::ostream& operator<<(std::ostream& os, const google_req_params& obj);
std::ostream& operator<<(std::ostream& os, const config& obj);

#endif // SETTINGS_STRUCTS_H
