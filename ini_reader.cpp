#include "ini_reader.h"

ini_reader::ini_reader(const std::string& ini_file_name)
{
    boost::property_tree::ini_parser::read_ini(ini_file_name, pt_);

    const std::string http_section = "HTTP.";
    request_settings_.host_ = pt_.get<std::string>(http_section + "host");
    request_settings_.port_ = pt_.get<std::string>(http_section + "port");
    request_settings_.version_ = pt_.get<int>(http_section + "version");

    const std::string bot_section = "Bot.";
    request_settings_.token_ = pt_.get<std::string>(bot_section + "token");

    const std::string configuration_section = "Configuration.";
    processing_threads_count_ = pt_.get<int>(configuration_section + "processing_threads_count");

    BOOST_LOG_TRIVIAL(info) << "configuration file was read:"
                            << "\nhost = " << request_settings_.host_
                            << "\nport = " << request_settings_.port_
                            << "\nversion = " << request_settings_.version_
                            << "\ntoken = " << request_settings_.token_
                            << "\nprocessing_threads_count_ = " << processing_threads_count_;
}

ini_reader& ini_reader::instance()
{
    static ini_reader ini_reader_instance("config.ini");
    return ini_reader_instance;
}

int ini_reader::get_processing_threads_count() const noexcept
{
    return processing_threads_count_;
}

request_settings ini_reader::get_request_settings() const
{
    return request_settings_;
}
