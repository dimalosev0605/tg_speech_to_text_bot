add_library(ini_reader STATIC "")

target_sources(ini_reader
    PRIVATE ini_reader.cpp settings_structs.h settings_structs.cpp
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ini_reader.h)

target_include_directories(ini_reader
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(ini_reader
    PRIVATE
    ${Boost_LOG_LIBRARY})

add_subdirectory(tests)
