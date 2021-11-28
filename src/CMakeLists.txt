add_executable(echo_bot main.cpp)
add_subdirectory(ini_reader)
add_subdirectory(external_program_callers)
add_subdirectory(bot)
target_link_libraries(echo_bot bot ini_reader external_program_callers)

