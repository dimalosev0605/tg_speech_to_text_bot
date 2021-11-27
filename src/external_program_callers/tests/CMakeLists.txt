find_package(Boost 1.72 REQUIRED COMPONENTS unit_test_framework)

add_executable(sample_rate_extractor_tests sample_rate_extractor_tests.cpp)

target_link_libraries(sample_rate_extractor_tests
    PRIVATE
    external_program_callers
    ini_reader
    Boost::unit_test_framework
)

target_compile_definitions(sample_rate_extractor_tests
    PRIVATE
    BOOST_TEST_DYN_LINK
)

add_test(
    NAME sample_rate_extractor_tests
    COMMAND $<TARGET_FILE:sample_rate_extractor_tests>
)

set(OGG_TEST_FILES
   ${CMAKE_SOURCE_DIR}/src/external_program_callers/tests/16000.ogg
   ${CMAKE_SOURCE_DIR}/src/external_program_callers/tests/48000.ogg
)

foreach(file_i ${OGG_TEST_FILES})
    add_custom_command(TARGET sample_rate_extractor_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${file_i} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
endforeach(file_i)
