#ifndef UPDATES_RECEIVER_H
#define UPDATES_RECEIVER_H

#include <string>
#include <iostream>
#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <boost/log/trivial.hpp>
#include <boost/json.hpp>

#include "threadsafe_queue.h"

class updates_receiver
{
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;

    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    std::unique_ptr<boost::beast::ssl_stream<boost::beast::tcp_stream>> stream_;

    std::unique_ptr<boost::beast::http::request<boost::beast::http::empty_body>> request_;
    std::unique_ptr<boost::beast::http::response<boost::beast::http::string_body>> response_;
    std::unique_ptr<boost::beast::flat_buffer> buffer_;

    std::string host_;
    std::string port_;
    std::string token_;
    int version_;
    std::string method_;
    std::string target_;

    bool is_offset_ = false;
    std::int64_t offset_;

    threadsafe_queue queue_;

private:
    void reset();
    int calculate_offset(boost::json::array& updates);
    std::string get_method() const;
    std::string get_target() const;

public:
    explicit updates_receiver(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context);

    void set_host(const std::string& host);
    void set_port(const std::string& port);
    void set_token(const std::string& token);
    void set_version(int version) noexcept;
    void set_method(const std::string& method);

    void run();

    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
};
// net::make_strand in my_first_bot_async/main.cpp
// Are ssl_context and io_context are thread safe?
// bind_front_handler params passing
// callbacks params passing
// add logger
// analyze stack call

#endif // UPDATES_RECEIVER_H
