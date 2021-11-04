#ifndef REQUEST_SETTINGS_H
#define REQUEST_SETTINGS_H

#include <string>

struct request_settings
{
    std::string host_;
    std::string port_;
    int version_;
    std::string token_;
};

#endif // REQUEST_SETTINGS_H
