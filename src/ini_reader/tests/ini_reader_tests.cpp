#include "ini_reader.h"
#include <type_traits>

#define BOOST_TEST_MODULE ini_reader_tests
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(telegram) {
    const auto tg_req_params = ini_reader::instance().get_tg_req_params();

    std::string host;
    std::string port;
    int version;
    std::string token;
    std::string bot_name;

    bool is_ok = true;

    is_ok = std::is_same_v<decltype(host), decltype(tg_req_params.host_)> && is_ok;
    is_ok = std::is_same_v<decltype(port), decltype(tg_req_params.port_)> && is_ok;
    is_ok = std::is_same_v<decltype(version), decltype(tg_req_params.version_)> && is_ok;
    is_ok = std::is_same_v<decltype(token), decltype(tg_req_params.token_)> && is_ok;
    is_ok = std::is_same_v<decltype(bot_name), decltype(tg_req_params.bot_name_)> && is_ok;

    BOOST_REQUIRE(is_ok);
}

BOOST_AUTO_TEST_CASE(google) {
    const auto google_req_params = ini_reader::instance().get_google_req_params();

    std::string host;
    std::string port;
    int version;
    std::string method;
    std::string key;
    std::string service_account_key_path;
    int access_token_generation_interval;

    bool is_ok = true;

    is_ok = std::is_same_v<decltype(host), decltype(google_req_params.host_)> && is_ok;
    is_ok = std::is_same_v<decltype(port), decltype(google_req_params.port_)> && is_ok;
    is_ok = std::is_same_v<decltype(version), decltype(google_req_params.version_)> && is_ok;
    is_ok = std::is_same_v<decltype(method), decltype(google_req_params.method_)> && is_ok;
    is_ok = std::is_same_v<decltype(key), decltype(google_req_params.key_)> && is_ok;
    is_ok = std::is_same_v<decltype(access_token_generation_interval), decltype(google_req_params.access_token_generation_interval_)> && is_ok;

    BOOST_REQUIRE(is_ok);
}

BOOST_AUTO_TEST_CASE(configuration) {
    const auto configuration = ini_reader::instance().get_configuration();

    std::string voice_messages_path;
    int processing_threads_count;
    int io_context_threads_count;
    std::string ca_certificates_file;
    int enabled_users_read_interval;

    bool is_ok = true;

    is_ok = std::is_same_v<decltype(voice_messages_path), decltype(configuration.voice_messages_path_)> && is_ok;
    is_ok = std::is_same_v<decltype(processing_threads_count), decltype(configuration.processing_threads_count_)> && is_ok;
    is_ok = std::is_same_v<decltype(io_context_threads_count), decltype(configuration.io_context_threads_count_)> && is_ok;
    is_ok = std::is_same_v<decltype(ca_certificates_file), decltype(configuration.ca_certificates_file_)> && is_ok;
    is_ok = std::is_same_v<decltype(enabled_users_read_interval), decltype(configuration.enabled_users_read_interval_)> && is_ok;

    BOOST_REQUIRE(is_ok);
}
