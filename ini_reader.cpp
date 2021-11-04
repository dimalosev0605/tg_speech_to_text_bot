#include "ini_reader.h"

ini_reader::ini_reader(const std::string& ini_file_name)
{
    boost::property_tree::ini_parser::read_ini(ini_file_name, pt_);

    const std::string http_section = "HTTP.";
    host_ = pt_.get<std::string>(http_section + "host");
    port_ = pt_.get<std::string>(http_section + "port");
    version_ = pt_.get<int>(http_section + "version");

    const std::string bot_section = "Bot.";
    token_ = pt_.get<std::string>(bot_section + "token");
    updates_method_ = pt_.get<std::string>(bot_section + "updates_method");

    BOOST_LOG_TRIVIAL(info) << "configuration file was read:"
                            << "\nhost = " << host_
                            << "\nport = " << port_
                            << "\nversion = " << version_
                            << "\ntoken = " << token_
                            << "\nupdates_method_ = " << updates_method_;
}

ini_reader& ini_reader::instance()
{
    static ini_reader ini_reader_instance("config.ini");
    return ini_reader_instance;
}

std::string ini_reader::get_host() const
{
    return host_;
}

std::string ini_reader::get_port() const
{
    return port_;
}

int ini_reader::get_version() const
{
    return version_;
}

std::string ini_reader::get_token() const
{
    return token_;
}

std::string ini_reader::get_updates_method() const
{
    return updates_method_;
}
