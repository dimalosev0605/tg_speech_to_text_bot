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

add_executable(enabled_users_tests enabled_users_tests.cpp)

target_link_libraries(enabled_users_tests
    PRIVATE
    bot
    ini_reader
    Boost::unit_test_framework
)

target_compile_definitions(enabled_users_tests
    PRIVATE
    BOOST_TEST_DYN_LINK
)

add_test(
    NAME enabled_users_tests
    COMMAND $<TARGET_FILE:enabled_users_tests>
)

set(ENABLED_USERS_TEST_FILES
   ${CMAKE_SOURCE_DIR}/src/bot/tests/use_enabled_users.json
   ${CMAKE_SOURCE_DIR}/src/bot/tests/not_use_enabled_users.json
)

foreach(file_i ${ENABLED_USERS_TEST_FILES})
    add_custom_command(TARGET enabled_users_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${file_i} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
endforeach(file_i)
