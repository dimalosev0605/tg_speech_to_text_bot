#include "updates_receiver.h"

updates_receiver::updates_receiver(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context)
    : io_context_{io_context},
      ssl_context_{ssl_context},
      resolver_{std::make_unique<boost::asio::ip::tcp::resolver>(io_context)},
      stream_{std::make_unique<boost::beast::ssl_stream<boost::beast::tcp_stream>>(io_context, ssl_context)},
      request_{std::make_unique<boost::beast::http::request<boost::beast::http::empty_body>>()},
      response_{std::make_unique<boost::beast::http::response<boost::beast::http::string_body>>()},
      buffer_{std::make_unique<boost::beast::flat_buffer>()}
{}

void updates_receiver::reset()
{
    resolver_ = std::make_unique<boost::asio::ip::tcp::resolver>(io_context_);
    stream_ = std::make_unique<boost::beast::ssl_stream<boost::beast::tcp_stream>>(io_context_, ssl_context_);
    request_ = std::make_unique<boost::beast::http::request<boost::beast::http::empty_body>>();
    response_ = std::make_unique<boost::beast::http::response<boost::beast::http::string_body>>();
    buffer_ = std::make_unique<boost::beast::flat_buffer>();
}

void updates_receiver::set_host(const std::string& host)
{
    host_ = host;
}

void updates_receiver::set_port(const std::string& port)
{
    port_ = port;
}

void updates_receiver::set_token(const std::string& token)
{
    token_ = token;
}

void updates_receiver::set_version(int version) noexcept
{
    version_ = version;
}

void updates_receiver::set_method(const std::string& method)
{
    method_ = method;
}

void updates_receiver::run()
{
    if(!SSL_set_tlsext_host_name(stream_->native_handle(), host_.c_str()))
    {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    target_ = "/bot" + token_ + "/" + method_;

    request_->version(version_);
    request_->method(boost::beast::http::verb::get);
    request_->target(target_);
    request_->set(boost::beast::http::field::host, host_);
    request_->set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    resolver_->async_resolve(host_.c_str(), port_.c_str(), boost::beast::bind_front_handler(&updates_receiver::on_resolve, this));
}

void updates_receiver::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }
//    boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
    boost::beast::get_lowest_layer(*stream_).async_connect(results, boost::beast::bind_front_handler(&updates_receiver::on_connect, this));
}

void updates_receiver::on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }

    stream_->async_handshake(boost::asio::ssl::stream_base::client, boost::beast::bind_front_handler(&updates_receiver::on_handshake, this));
}

void updates_receiver::on_handshake(boost::beast::error_code ec)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }
//    boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
    boost::beast::http::async_write(*stream_, *request_, boost::beast::bind_front_handler(&updates_receiver::on_write, this));
}

void updates_receiver::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }

    boost::beast::http::async_read(*stream_, *buffer_, *response_, boost::beast::bind_front_handler(&updates_receiver::on_read, this));
}

void updates_receiver::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }

//    BOOST_LOG_TRIVIAL(info) << *response_;
//    std::this_thread::sleep_for(std::chrono::milliseconds(500));
//    std::cout << *response_ << std::endl;
//    boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    //calculate offset, push updates to queue, clear buffers, check for closing connection.

    const auto connection = response_->base()["Connection"];
    *response_ = {};

    if(connection == "close") {
        BOOST_LOG_TRIVIAL(warning) << "server close connetion" << std::endl;
        reset();
        run();
    } else {
        boost::beast::http::async_write(*stream_, *request_, boost::beast::bind_front_handler(&updates_receiver::on_write, this));
    }
}

