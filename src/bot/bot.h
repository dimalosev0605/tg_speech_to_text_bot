#ifndef BOT_H
#define BOT_H

#include "updates_receiver.h"
#include "updates_processor.h"

class bot
{
    const config config_;

    boost::asio::io_context io_context_;
    boost::asio::ssl::context ssl_context_;
    std::vector<std::thread> io_context_threads_;
    boost::asio::io_context::basic_executor_type<std::allocator<void>, 4> work_;

    boost::asio::signal_set signals_;

    threadsafe_queue queue_;
    updates_receiver updates_receiver_;
    std::vector<std::unique_ptr<updates_processor>> updates_processors_;

public:
    explicit bot();

    bot(const bot&) = delete;
    bot& operator=(const bot&) = delete;

    bot(bot&&) = delete;
    bot& operator=(bot&&) = delete;

    ~bot();

    void start();
};

#endif // BOT_H
