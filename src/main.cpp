#include "bot.h"
#include "logger.h"

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{
    init_log();
    BOOST_LOG_TRIVIAL(info) << "launch";

    bot bot;
    bot.start();

    return 0;
}
