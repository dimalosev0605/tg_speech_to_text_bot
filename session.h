#ifndef SESSION_H
#define SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <boost/log/trivial.hpp>

#include "request_settings.h"

class session : public std::enable_shared_from_this<session>
{
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::http::request<boost::beast::http::empty_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
    boost::beast::flat_buffer buffer_;
    request_settings request_settings_;
    std::string method_;

private:
    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_shutdown(boost::beast::error_code ec);

    std::string get_target() const;

public:
    explicit session(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context,
                     const request_settings& request_settings, const std::string& method);
    void run();
};

#endif // SESSION_H
