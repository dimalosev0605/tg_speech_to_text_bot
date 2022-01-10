#include "settings_structs.h"

std::ostream& operator<<(std::ostream& os, const tg_req_params& obj)
{
    os << "Telegram:"
    << "\nhost = " << obj.host_
    << "\nport = " << obj.port_
    << "\nversion = " << obj.version_
    << "\ntoken = " << obj.token_
    << "\nbot_name = " << obj.bot_name_
    << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const google_req_params& obj)
{
    os << "Google:"
    << "\nhost = " << obj.host_
    << "\nport = " << obj.port_
    << "\nversion = " << obj.version_
    << "\nkey = " << obj.key_
    << "\nmethod = " << obj.method_
    << "\nservice_account_key_path = " << obj.service_account_key_path_
    << "\naccess_token_generation_interval = " << obj.access_token_generation_interval_
    << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const config& obj)
{
    os << "Configuration:"
    << "\nprocessing_threads_count = " << obj.processing_threads_count_
    << "\nio_context_threads_count = " << obj.io_context_threads_count_
    << "\nvoice_messages_path = " << obj.voice_messages_path_
    << "\nca_certificates_file = " << obj.ca_certificates_file_
    << "\nenabled_users_read_interval = " << obj.enabled_users_read_interval_
    << "\n";
    return os;
}
