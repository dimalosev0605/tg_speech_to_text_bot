cmake_minimum_required(VERSION 3.5)

project(echo_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

find_package(Boost 1.77 REQUIRED COMPONENTS log log_setup REQUIRED)

add_executable(${PROJECT_NAME} main.cpp updates_receiver.h updates_receiver.cpp logger.h logger.cpp ini_reader.h ini_reader.cpp threadsafe_queue.h threadsafe_queue.cpp)
target_link_libraries(${PROJECT_NAME} "-pthread" "-lcrypto" "-lssl" "-lboost_log" "-lboost_log_setup" "-lboost_thread")
