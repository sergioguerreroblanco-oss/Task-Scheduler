cmake_minimum_required(VERSION 3.21)

project(TaskScheduler 
    VERSION 1.0.0
    DESCRIPTION "Multithreaded Task Scheduler in C++14"
    HOMEPAGE_URL "https://github.com/sergioguerreroblanco-oss/Task_Scheduler"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------
# Enable testing framework
# -----------------------------------------------------------
include(CTest)

# -----------------------------------------------------------
# Core library
# -----------------------------------------------------------
add_library(core STATIC
    src/job_queue.cpp
    src/logger.cpp
    src/print_job.cpp
    src/thread_pool.cpp)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/optional-lite/include
)

if(MSVC)
    target_compile_options(core PRIVATE /W4 /permissive- /wd26495)
else()
    target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------
# Main executable
# -----------------------------------------------------------
add_executable(task_scheduler src/main.cpp)
target_link_libraries(task_scheduler PRIVATE core)

# -----------------------------------------------------------
# Tests
# -----------------------------------------------------------
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests 
        tests/test_main.cpp 
        tests/test_jobs.cpp
        tests/test_job_queue.cpp
        tests/test_thread_pool.cpp
        tests/fake_job.h 
        tests/fake_slow_job.h
        tests/fake_throwing_job.h)
    target_link_libraries(tests PRIVATE core gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
