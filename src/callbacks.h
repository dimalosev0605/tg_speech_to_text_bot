#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/log/trivial.hpp>

#include "threadsafe_queue.h"
#include "ini_reader.h"

enum class actions : int
{
    download_voice_from_tg,
    google_recognize,
    send_voice_transcript,
};

void prepare_voice_downloading_from_tg(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info);
void download_voice_from_tg(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info);
void google_recognize(threadsafe_queue& queue, boost::beast::http::response<boost::beast::http::string_body>& response, boost::json::object chat_info);

#endif // CALLBACKS_H
