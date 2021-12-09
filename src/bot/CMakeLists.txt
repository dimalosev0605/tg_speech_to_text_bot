add_library(bot STATIC "")

find_package(SQLite3 REQUIRED)

target_sources(bot
    PRIVATE
    updates_receiver.cpp logger.cpp threadsafe_queue.cpp updates_processor.cpp session.cpp callbacks.cpp bot.cpp enabled_users.cpp chats_settings.cpp bot_commands_processor.cpp bot_utility.cpp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/updates_receiver.h ${CMAKE_CURRENT_LIST_DIR}/logger.h ${CMAKE_CURRENT_LIST_DIR}/threadsafe_queue.h
    ${CMAKE_CURRENT_LIST_DIR}/updates_processor.h ${CMAKE_CURRENT_LIST_DIR}/session.h ${CMAKE_CURRENT_LIST_DIR}/callbacks.h
    ${CMAKE_CURRENT_LIST_DIR}/bot.h ${CMAKE_CURRENT_LIST_DIR}/enabled_users.h ${CMAKE_CURRENT_LIST_DIR}/chats_settings.h
    ${CMAKE_CURRENT_LIST_DIR}/bot_commands_processor.h ${CMAKE_CURRENT_LIST_DIR}/bot_utility.h)

target_include_directories(bot
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(bot
    PRIVATE
    ini_reader
    external_program_callers
    ${Boost_LOG_SETUP_LIBRARY}
    "-lcrypto"
    "-lssl"
    SQLite::SQLite3)

add_subdirectory(tests)
