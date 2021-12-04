#include "bot.h"

bot::bot()
    : config_{ini_reader::instance().get_configuration()},
      io_context_{config_.io_context_threads_count_},
      ssl_context_{boost::asio::ssl::context::tlsv12_client},
      work_{boost::asio::require(io_context_.get_executor(), boost::asio::execution::outstanding_work.tracked)},
      signals_{io_context_, SIGINT},
      updates_receiver_{io_context_, ssl_context_, queue_}
{
    ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer);
    ssl_context_.load_verify_file(config_.ca_certificates_file_);
}

bot::~bot()
{
    for(std::size_t i = 0; i < io_context_threads_.size(); ++i) {
        io_context_threads_[i].join();
    }
    BOOST_LOG_TRIVIAL(info) << "successful exit";
}

void bot::start()
{
    updates_processors_.reserve(config_.processing_threads_count_);
    for(int i = 0; i < config_.processing_threads_count_; ++i) {
        updates_processors_.push_back(std::make_unique<updates_processor>(io_context_, ssl_context_, queue_, enabled_users_));
        updates_processors_[i]->run();
    }
    updates_receiver_.run();

    signals_.async_wait([this](boost::beast::error_code const&, int){
        io_context_.stop();
    });

    io_context_threads_.reserve(config_.io_context_threads_count_);
    for(int i = 0; i < config_.io_context_threads_count_; ++i) {
        io_context_threads_.emplace_back([this](){
            io_context_.run();
        });
    }
}

