#include "session.h"

session::session(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, threadsafe_queue& queue,
                 boost::beast::http::request<boost::beast::http::string_body> request,
                 std::function<void (threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info)> callback,
                 const boost::json::object& chat_info)
    : resolver_{io_context},
      stream_{io_context, ssl_context},
      request_{request},
      queue_{queue},
      callback_{callback},
      chat_info_{chat_info}
{}

void session::run(service service)
{
    std::string host;
    std::string port;
    switch (service) {
    case service::google: {
        host = ini_reader::instance().get_google_req_params().host_;
        port = ini_reader::instance().get_google_req_params().port_;
        break;
    }
    case service::telegram: {
        host = ini_reader::instance().get_tg_req_params().host_;
        port = ini_reader::instance().get_tg_req_params().port_;
        break;
    }
    }

    if(!SSL_set_tlsext_host_name(stream_.native_handle(), host.c_str()))
    {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    resolver_.async_resolve(host.c_str(), port.c_str(), boost::beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}

void session::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }
    boost::beast::get_lowest_layer(stream_).async_connect(results, boost::beast::bind_front_handler(&session::on_connect, shared_from_this()));
}

void session::on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    stream_.async_handshake(boost::asio::ssl::stream_base::client, boost::beast::bind_front_handler(&session::on_handshake, shared_from_this()));
}

void session::on_handshake(boost::beast::error_code ec)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }
    boost::beast::http::async_write(stream_, request_, boost::beast::bind_front_handler(&session::on_write, shared_from_this()));
}

void session::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }
    boost::beast::http::async_read(stream_, buffer_, response_, boost::beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    if(callback_) {
        callback_(queue_, response_, chat_info_);
    }

    stream_.async_shutdown(boost::beast::bind_front_handler(&session::on_shutdown, shared_from_this()));
}

void session::on_shutdown(boost::beast::error_code ec)
{
    if(ec && ec != boost::asio::ssl::error::stream_truncated) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
    }
}
