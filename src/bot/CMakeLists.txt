add_library(bot STATIC "")

target_sources(bot
    PRIVATE
    updates_receiver.cpp logger.cpp threadsafe_queue.cpp updates_processor.cpp session.cpp callbacks.cpp base64.h
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/updates_receiver.h ${CMAKE_CURRENT_LIST_DIR}/logger.h ${CMAKE_CURRENT_LIST_DIR}/threadsafe_queue.h
    ${CMAKE_CURRENT_LIST_DIR}/updates_processor.h ${CMAKE_CURRENT_LIST_DIR}/session.h ${CMAKE_CURRENT_LIST_DIR}/callbacks.h)

target_include_directories(bot
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(bot
    PRIVATE
    ini_reader
    external_program_callers
    ${Boost_LOG_SETUP_LIBRARY}
    "-lcrypto"
    "-lssl")
