#include "updates_receiver.h"

#include <boost/json/src.hpp>

updates_receiver::updates_receiver(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context, const request_settings& request_settings)
    : io_context_{io_context},
      ssl_context_{ssl_context},
      resolver_{std::make_unique<boost::asio::ip::tcp::resolver>(io_context)},
      stream_{std::make_unique<boost::beast::ssl_stream<boost::beast::tcp_stream>>(io_context, ssl_context)},
      request_{std::make_unique<boost::beast::http::request<boost::beast::http::empty_body>>()},
      response_{std::make_unique<boost::beast::http::response<boost::beast::http::string_body>>()},
      buffer_{std::make_unique<boost::beast::flat_buffer>()},
      request_settings_{request_settings}
{}

void updates_receiver::reset()
{
    resolver_ = std::make_unique<boost::asio::ip::tcp::resolver>(io_context_);
    stream_ = std::make_unique<boost::beast::ssl_stream<boost::beast::tcp_stream>>(io_context_, ssl_context_);
    request_ = std::make_unique<boost::beast::http::request<boost::beast::http::empty_body>>();
    response_ = std::make_unique<boost::beast::http::response<boost::beast::http::string_body>>();
    buffer_ = std::make_unique<boost::beast::flat_buffer>();
    is_offset_ = false;
}

int updates_receiver::calculate_offset(boost::json::array& updates)
{
    std::int64_t max_update_id = INT64_MIN;
    for(auto& elem : updates) {
        auto update_obj = elem.as_object();
        auto update_id = update_obj["update_id"].as_int64();
        if(update_id > max_update_id) {
            max_update_id = update_id;
        }
    }
    is_offset_ = true;
    return max_update_id + 1;
}

std::string updates_receiver::get_method() const
{
    if(is_offset_) {
        return method_ + "?offset=" + std::to_string(offset_);
    }
    return method_;
}

std::string updates_receiver::get_target() const
{
    return "/bot" + request_settings_.token_ + "/" + get_method();
}

void updates_receiver::start_updates_processors(int processing_thread_count)
{
    updates_processors_.reserve(processing_thread_count);
    for(int i = 0; i < processing_thread_count; ++i) {
        updates_processors_.push_back(std::make_unique<updates_processor>(io_context_, ssl_context_, queue_));
        updates_processors_[i]->run();
    }
}

void updates_receiver::run()
{
    if(!SSL_set_tlsext_host_name(stream_->native_handle(), request_settings_.host_.c_str()))
    {
        boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        return;
    }

    request_->version(request_settings_.version_);
    request_->method(boost::beast::http::verb::get);
    request_->target(get_target());
    request_->set(boost::beast::http::field::host, request_settings_.host_);
    request_->set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    resolver_->async_resolve(request_settings_.host_.c_str(), request_settings_.port_.c_str(), boost::beast::bind_front_handler(&updates_receiver::on_resolve, this));
}

void updates_receiver::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if(ec) {
        BOOST_LOG_TRIVIAL(error) << ec.message() << std::endl;
        reset();
        run();
        return;
    }
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

    auto body_obj = boost::json::parse(response_->body().data()).as_object();

    auto ok = body_obj["ok"].as_bool();
    if(ok) {
        auto result = body_obj["result"].as_array();
        if(!result.empty()) {
            queue_.push(result);
            offset_ = calculate_offset(result);
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "unsuccessful request, response =\n" << *response_;
    }

    const auto connection = response_->base()["Connection"];
    if(connection == "close") {
        BOOST_LOG_TRIVIAL(warning) << "server close connetion" << std::endl;
        reset();
        run();
    } else {
        request_->target(get_target());
        boost::beast::http::async_write(*stream_, *request_, boost::beast::bind_front_handler(&updates_receiver::on_write, this));
    }

    *response_ = {};
}
