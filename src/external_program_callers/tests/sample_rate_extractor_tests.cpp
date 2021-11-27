#include <boost/json/src.hpp>

#include "sample_rate_extractor.h"
#include "gcloud_at_gen.h"

#define BOOST_TEST_MODULE sample_rate_extractor_tests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(extract_sample_rate_16khz) {
    const auto sample_rate = sample_rate_extractor::instanse().extract_sample_rate("./16000.ogg");
    BOOST_REQUIRE(sample_rate == 16000);
}

BOOST_AUTO_TEST_CASE(extract_sample_rate_48khz) {
    const auto sample_rate = sample_rate_extractor::instanse().extract_sample_rate("./48000.ogg");
    BOOST_REQUIRE(sample_rate == 48000);
}

BOOST_AUTO_TEST_CASE(generate_gcloud_access_token) {
    const auto access_token = gcloud_at_gen::instance().generate_access_token();
    BOOST_REQUIRE(access_token != "");
}
