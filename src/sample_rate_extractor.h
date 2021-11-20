#ifndef SAMPLE_RATE_EXTRACTOR_H
#define SAMPLE_RATE_EXTRACTOR_H

#include <boost/process.hpp>
#include <boost/json.hpp>

class sample_rate_extractor
{
public:
    int extract_sample_rate(const std::string& file_path);
};

#endif // SAMPLE_RATE_EXTRACTOR_H
