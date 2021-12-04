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
#include "ini_reader.h"

class updates_receiver
{
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;
    threadsafe_queue& queue_;

    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    std::unique_ptr<boost::beast::ssl_stream<boost::beast::tcp_stream>> stream_;
    std::unique_ptr<boost::beast::http::request<boost::beast::http::empty_body>> request_;
    std::unique_ptr<boost::beast::http::response<boost::beast::http::string_body>> response_;
    std::unique_ptr<boost::beast::flat_buffer> buffer_;

    const std::string method_ = "getUpdates";
    std::string target_;

    bool is_offset_ = false;
    std::int64_t offset_;

private:
    void reset();
    int calculate_offset(boost::json::array& updates);
    std::string get_method() const;
    std::string get_target() const;

    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);

public:
    explicit updates_receiver(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue);

    updates_receiver(const updates_receiver&) = delete;
    updates_receiver& operator=(const updates_receiver&) = delete;

    updates_receiver(updates_receiver&&) = delete;
    updates_receiver& operator=(updates_receiver&&) = delete;

    void run();

};

#endif // UPDATES_RECEIVER_H
