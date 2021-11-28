#include "logger.h"

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

void init_log()
{
    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);

    // log format: [time_stamp] [thread_id] [severity_level] log message
    auto fmt_time_stamp = boost::log::expressions:: format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
    auto fmt_thread_id = boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");
    auto fmt_severity = boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity");
    boost::log::formatter log_fmt =
        boost::log::expressions::format("[%1%] [%2%] [%3%] %4%")
        % fmt_time_stamp % fmt_thread_id % fmt_severity
        % boost::log::expressions::smessage;

    auto console_sink = boost::log::add_console_log(std::cout, boost::log::keywords::auto_flush = true);
    console_sink->set_formatter(log_fmt);

    auto fs_sink = boost::log::add_file_log(boost::log::keywords::file_name = "log_%Y-%m-%d_%H-%M-%S.%N.log");
    fs_sink->set_formatter(log_fmt);
    fs_sink->locked_backend()->auto_flush(true);
}
