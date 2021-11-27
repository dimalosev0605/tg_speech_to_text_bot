#include "sample_rate_extractor.h"

sample_rate_extractor& sample_rate_extractor::instanse()
{
    static sample_rate_extractor sample_rate_extractor_instanse;
    return sample_rate_extractor_instanse;
}

int sample_rate_extractor::extract_sample_rate(const std::string& file_path)
{
    const std::string command = "mediainfo --Output=JSON " + file_path;
    boost::process::ipstream pipe_stream;
    boost::process::child c(command, boost::process::std_out > pipe_stream);
    std::vector<std::string> data;
    std::string line;
    while (c.running() && std::getline(pipe_stream, line) && !line.empty()) {
        if(!line.empty()) {
            data.push_back(std::move(line));
        }
    }
    c.wait();
    if(c.exit_code() == 0) {
        std::string json_s;
        for(std::size_t i = 0; i < data.size(); ++i) {
            json_s += data[i];
        }
        auto obj = boost::json::parse(json_s).as_object();
        auto media = obj["media"].as_object();
        auto track = media["track"].as_array();
        if(track.size() == 2) {
            auto track_obj = track[1].as_object();
            auto sample_rate = std::stoi(track_obj["SamplingRate"].as_string().c_str());
            return sample_rate;
        }
    }
    return 0;
}
