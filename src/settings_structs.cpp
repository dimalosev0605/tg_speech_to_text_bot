#include "settings_structs.h"

std::ostream& operator<<(std::ostream& os, const tg_req_params& obj)
{
    os << "Telegram:"
    << "\nhost = " << obj.host_
    << "\nport = " << obj.port_
    << "\nversion = " << obj.version_
    << "\ntoken = " << obj.token_
    << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const google_req_params& obj)
{
    os << "Google:"
    << "\ngoogle_host = " << obj.host_
    << "\ngoogle_port = " << obj.port_
    << "\ngoogle_version = " << obj.version_
    << "\ngoogle_key = " << obj.key_
    << "\ngoogle_method = " << obj.method_
    << "\nservice_account_key_path = " << obj.service_account_key_path_
    << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const config& obj)
{
    os << "Configuration:"
    << "\nprocessing_threads_count = " << obj.processing_threads_count_
    << "\nio_context_threads_count = " << obj.io_context_threads_count_
    << "\nvoice_messages_path = " << obj.voice_messages_path_
    << "\n";
    return os;
}
