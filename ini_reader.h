#ifndef INI_READER_H
#define INI_READER_H

#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "request_settings.h"

class ini_reader
{
    boost::property_tree::ptree pt_;
    request_settings request_settings_;
    int processing_threads_count_;

private:
    explicit ini_reader(const std::string& ini_file_name);

public:
    static ini_reader& instance();

    ini_reader(const ini_reader&) = delete;
    ini_reader& operator=(const ini_reader&) = delete;

    ini_reader(ini_reader&&) = delete;
    ini_reader& operator=(ini_reader&&) = delete;

    int get_processing_threads_count() const noexcept;
    request_settings get_request_settings() const;
};

#endif // INI_READER_H
