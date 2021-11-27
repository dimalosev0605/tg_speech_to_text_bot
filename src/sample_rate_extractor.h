#ifndef SAMPLE_RATE_EXTRACTOR_H
#define SAMPLE_RATE_EXTRACTOR_H

#include <boost/process.hpp>
#include <boost/json.hpp>

class sample_rate_extractor
{

private:
    sample_rate_extractor() = default;

public:
    static sample_rate_extractor& instanse();

    sample_rate_extractor(const sample_rate_extractor&) = delete;
    sample_rate_extractor& operator=(const sample_rate_extractor&) = delete;

    sample_rate_extractor(sample_rate_extractor&&) = delete;
    sample_rate_extractor& operator=(sample_rate_extractor&&) = delete;

    int extract_sample_rate(const std::string& file_path);
};

#endif // SAMPLE_RATE_EXTRACTOR_H
