#include "gcloud_at_gen.h"

gcloud_at_gen::gcloud_at_gen()
    : access_token_generation_interval_{ini_reader::instance().get_google_req_params().access_token_generation_interval_}
{
}

gcloud_at_gen& gcloud_at_gen::instance()
{
    static gcloud_at_gen gcloud_at_gen_instance;
    return gcloud_at_gen_instance;
}

std::string gcloud_at_gen::generate_access_token()
{
    std::lock_guard<std::mutex> lock(m_);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - prev_gen_time_;
    if(elapsed_seconds.count() > access_token_generation_interval_) {
        boost::process::ipstream pipe_stream;
        auto env = boost::this_process::environment();
        env["GOOGLE_APPLICATION_CREDENTIALS"] = ini_reader::instance().get_google_req_params().service_account_key_path_;
        boost::process::child c("gcloud auth application-default print-access-token", env, boost::process::std_out > pipe_stream);
        std::string line;
        while (c.running() && std::getline(pipe_stream, line) && !line.empty()) {
            if(!line.empty()) {
                prev_gen_time_ = now;
                access_token_ = line;
            }
        }
        c.wait();
        if(c.exit_code() == 0) {
            return access_token_;
        }
        return "";
    }
    else {
        return access_token_;
    }
}
