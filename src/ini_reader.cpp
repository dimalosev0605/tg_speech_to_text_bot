#include "ini_reader.h"

ini_reader::ini_reader(const std::string& ini_file_name)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file_name, pt);

    const std::string telegram_section = "Telegram.";
    tg_req_params_.host_ = pt.get<std::string>(telegram_section + "host");
    tg_req_params_.port_ = pt.get<std::string>(telegram_section + "port");
    tg_req_params_.version_ = pt.get<int>(telegram_section + "version");
    tg_req_params_.token_ = pt.get<std::string>(telegram_section + "token");

    const std::string configuration_section = "Configuration.";
    config_.processing_threads_count_ = pt.get<int>(configuration_section + "processing_threads_count");
    config_.io_context_threads_count_ = pt.get<int>(configuration_section + "io_context_threads_count");
    config_.voice_messages_path_ = pt.get<std::string>(configuration_section + "voice_messages_path");

    const std::string google_section = "Google.";
    google_req_params_.host_ = pt.get<std::string>(google_section + "host");
    google_req_params_.port_ = pt.get<std::string>(google_section + "port");
    google_req_params_.version_ = pt.get<int>(google_section + "version");
    google_req_params_.key_ = pt.get<std::string>(google_section + "key");
    google_req_params_.method_ = pt.get<std::string>(google_section + "method");
    google_req_params_.service_account_key_path_ = pt.get<std::string>(google_section + "service_account_key_path");
    google_req_params_.access_token_generation_interval_ = pt.get<int>(google_section + "access_token_generation_interval");

    BOOST_LOG_TRIVIAL(info) << "Configuration file was read:\n"
                            << tg_req_params_
                            << google_req_params_
                            << config_;
}

ini_reader& ini_reader::instance()
{
    static ini_reader ini_reader_instance("config.ini");
    return ini_reader_instance;
}

config ini_reader::get_configuration() const
{
    return config_;
}

tg_req_params ini_reader::get_tg_req_params() const
{
    return tg_req_params_;
}

google_req_params ini_reader::get_google_req_params() const
{
    return google_req_params_;
}
