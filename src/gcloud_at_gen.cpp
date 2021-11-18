#include "gcloud_at_gen.h"

std::string gcloud_at_gen::generate_access_token()
{
    boost::process::ipstream pipe_stream;
    auto env = boost::this_process::environment();
    env["GOOGLE_APPLICATION_CREDENTIALS"] = ini_reader::instance().get_google_req_params().service_account_key_path_;
    boost::process::child c("gcloud auth application-default print-access-token", env, boost::process::std_out > pipe_stream);
    std::string line;
    std::string access_token;
    while (c.running() && std::getline(pipe_stream, line) && !line.empty()) {
        if(!line.empty()) {
            access_token = line;
        }
    }
    c.wait();
    if(c.exit_code() == 0) {
        return access_token;
    }
    return "";
}
