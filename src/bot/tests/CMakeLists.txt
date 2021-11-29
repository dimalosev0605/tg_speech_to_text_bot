find_package(Boost 1.72 REQUIRED COMPONENTS unit_test_framework)

add_executable(threadsafe_queue_tests threadsafe_queue_tests.cpp)

target_link_libraries(threadsafe_queue_tests
    PRIVATE
    bot
    Boost::unit_test_framework
)

target_compile_definitions(threadsafe_queue_tests
    PRIVATE
    BOOST_TEST_DYN_LINK
)

add_test(
    NAME threadsafe_queue_tests
    COMMAND $<TARGET_FILE:threadsafe_queue_tests>
)
