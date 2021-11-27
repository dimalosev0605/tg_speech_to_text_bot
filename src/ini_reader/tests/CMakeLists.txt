find_package(Boost 1.72 REQUIRED COMPONENTS unit_test_framework)

add_executable(ini_reader_tests ini_reader_tests.cpp)

target_link_libraries(ini_reader_tests
    PRIVATE
    ini_reader
    Boost::unit_test_framework
)

target_compile_definitions(ini_reader_tests
    PRIVATE
    BOOST_TEST_DYN_LINK
)

add_test(
    NAME ini_reader_tests
    COMMAND $<TARGET_FILE:ini_reader_tests>
)

add_custom_command(TARGET ini_reader_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/config.ini
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config.ini
)
