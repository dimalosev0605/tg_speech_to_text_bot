#include "/home/dmitry/boost_1_77_0/libs/beast/example/common/root_certificates.hpp"

#include <boost/asio/signal_set.hpp>

#include "bot/updates_receiver.h"
#include "bot/logger.h"
#include "ini_reader/ini_reader.h"

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{
    init_log();
    BOOST_LOG_TRIVIAL(info) << "launch";

    boost::asio::io_context io_context{ini_reader::instance().get_configuration().io_context_threads_count_};
    boost::asio::ssl::context ssl_context{boost::asio::ssl::context::tlsv12_client};
    load_root_certificates(ssl_context);
    ssl_context.set_verify_mode(boost::asio::ssl::verify_peer);

    updates_receiver updates_receiver{io_context, ssl_context};
    updates_receiver.start_updates_processors(ini_reader::instance().get_configuration().processing_threads_count_);
    updates_receiver.run();

    boost::asio::signal_set signals(io_context, SIGINT);
    signals.async_wait([&io_context](boost::beast::error_code const&, int){
        io_context.stop();
    });

    auto work = boost::asio::require(io_context.get_executor(), boost::asio::execution::outstanding_work.tracked);

    std::vector<std::thread> io_context_threads;
    io_context_threads.reserve(ini_reader::instance().get_configuration().io_context_threads_count_);
    for(int i = 0; i < ini_reader::instance().get_configuration().io_context_threads_count_; ++i) {
        io_context_threads.emplace_back([&io_context](){
            io_context.run();
        });
    }

    for(int i = 0; i < ini_reader::instance().get_configuration().io_context_threads_count_; ++i) {
        io_context_threads[i].join();
    }

    BOOST_LOG_TRIVIAL(info) << "successful exit";
    return 0;
}
