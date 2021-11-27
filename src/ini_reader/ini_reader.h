#ifndef INI_READER_H
#define INI_READER_H

#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "settings_structs.h"

class ini_reader
{
    tg_req_params tg_req_params_;
    google_req_params google_req_params_;
    config config_;

private:
    explicit ini_reader(const std::string& ini_file_name);

public:
    static ini_reader& instance();

    ini_reader(const ini_reader&) = delete;
    ini_reader& operator=(const ini_reader&) = delete;

    ini_reader(ini_reader&&) = delete;
    ini_reader& operator=(ini_reader&&) = delete;

    config get_configuration() const;
    tg_req_params get_tg_req_params() const;
    google_req_params get_google_req_params() const;
};

#endif // INI_READER_H
