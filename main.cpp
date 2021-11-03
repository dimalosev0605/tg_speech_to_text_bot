#include "/home/dmitry/boost_1_77_0/libs/beast/example/common/root_certificates.hpp"

#include "updates_receiver.h"
#include "logger.h"
#include "ini_reader.h"

int main(int argc, char** argv)
{
    init_log();
    BOOST_LOG_TRIVIAL(info) << "launch";

    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context{boost::asio::ssl::context::tlsv12_client};
    load_root_certificates(ssl_context);
    ssl_context.set_verify_mode(boost::asio::ssl::verify_peer);

    updates_receiver updates_receiver{io_context, ssl_context};
    updates_receiver.set_host(ini_reader::instance().get_host());
    updates_receiver.set_port(ini_reader::instance().get_port());
    updates_receiver.set_version(ini_reader::instance().get_version());

    updates_receiver.set_token(ini_reader::instance().get_token());
    updates_receiver.set_method(ini_reader::instance().get_updates_method());

    auto work = boost::asio::require(io_context.get_executor(), boost::asio::execution::outstanding_work.tracked);
    updates_receiver.run();
    io_context.run();

    return 0;
}
