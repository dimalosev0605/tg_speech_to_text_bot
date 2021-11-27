add_library(external_program_callers STATIC "")

target_sources(external_program_callers
    PRIVATE gcloud_at_gen.cpp sample_rate_extractor.cpp
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/gcloud_at_gen.h ${CMAKE_CURRENT_LIST_DIR}/sample_rate_extractor.h)

target_include_directories(external_program_callers
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(external_program_callers
    PRIVATE
    ini_reader)

add_subdirectory(tests)
