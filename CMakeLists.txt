cmake_minimum_required(VERSION 3.5)

project(zvukozavr_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

set(ASAN_UBSAN OFF)
if(ASAN_UBSAN)
    include(CheckCXXCompilerFlag)
    set(ASAN_UBSAN_FLAGS "-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fsanitize-recover=address")
    set(CMAKE_REQUIRED_FLAGS ${ASAN_UBSAN_FLAGS})
    check_cxx_compiler_flag(${ASAN_UBSAN_FLAGS} asan_ubsan_works)
    unset(CMAKE_REQUIRED_FLAGS)
    if(asan_ubsan_works)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fsanitize-recover=address")
    endif()
endif()

set(TSAN OFF)
if(TSAN)
    include(CheckCXXCompilerFlag)
    set(TSAN_FLAGS "-fsanitize=thread -fno-omit-frame-pointer -fPIE")
    set(CMAKE_REQUIRED_FLAGS ${TSAN_FLAGS})
    check_cxx_compiler_flag(${TSAN_FLAGS} tsan_works)
    unset(CMAKE_REQUIRED_FLAGS)

    if(tsan_works)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=thread -fno-omit-frame-pointer -fPIE -pie")
    endif()
endif()


find_package(Boost 1.77 REQUIRED COMPONENTS log_setup log thread REQUIRED)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

enable_testing()
add_subdirectory(src)
