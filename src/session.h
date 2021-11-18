#ifndef SESSION_H
#define SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <boost/log/trivial.hpp>
#include <boost/json.hpp>
#include <functional>

#include "ini_reader.h"
#include "threadsafe_queue.h"
#include "callbacks.h"

enum class service : int
{
    google,
    telegram
};

class session : public std::enable_shared_from_this<session>
{
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
    boost::beast::flat_buffer buffer_;
    threadsafe_queue& queue_;
    std::string target_;
    std::function<void (threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)> callback_;
    boost::json::object chat_info_;

private:
    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_shutdown(boost::beast::error_code ec);

public:
    explicit session(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue,
                     boost::beast::http::request<boost::beast::http::string_body> request,
                     std::function<void (threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)> callback = {},
                     const boost::json::object& chat_info = {});
    void run(service service);
};

#endif // SESSION_H
