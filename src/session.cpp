#include "session.h"

session::session(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context,
                 const request_settings& request_settings, const std::string& method)
    : resolver_{io_context},
      stream_{io_context, ssl_context},
      request_settings_{request_settings},
      method_{method}
{}

void session::run()
{
    if(!SSL_set_tlsext_host_name(stream_.native_handle(), request_settings_.host_.c_str()))
    {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    request_.version(request_settings_.version_);
    request_.method(boost::beast::http::verb::get);
    request_.target(get_target());
    request_.set(boost::beast::http::field::host, request_settings_.host_);
    request_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    resolver_.async_resolve(request_settings_.host_.c_str(), request_settings_.port_.c_str(), boost::beast::bind_front_handler(&session::on_resolve, shared_from_this()));
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
    stream_.async_shutdown(boost::beast::bind_front_handler(&session::on_shutdown, shared_from_this()));
}

void session::on_shutdown(boost::beast::error_code ec)
{
    if(ec && ec != boost::asio::ssl::error::stream_truncated) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
    }
}

std::string session::get_target() const
{
    return "/bot" + request_settings_.token_ + "/" + method_;
}
