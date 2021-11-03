#ifndef INI_READER_H
#define INI_READER_H

#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

class ini_reader
{
    boost::property_tree::ptree pt_;

    std::string host_;
    std::string port_;
    int version_;

    std::string token_;
    std::string updates_method_;

private:
    explicit ini_reader(const std::string& ini_file_name);

public:
    static ini_reader& instance();

    ini_reader(const ini_reader&) = delete;
    ini_reader& operator=(const ini_reader&) = delete;

    ini_reader(ini_reader&&) = delete;
    ini_reader& operator=(ini_reader&&) = delete;

    std::string get_host() const;
    std::string get_port() const;
    int get_version() const;

    std::string get_token() const;
    std::string get_updates_method() const;
};

#endif // INI_READER_H
