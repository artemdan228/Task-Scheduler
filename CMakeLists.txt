cmake_minimum_required(VERSION 3.14)
project(TaskScheduler)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/lib)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(main bin/main.cpp)

add_executable(tests
        tests/scheduler_test.cpp
)

target_link_libraries(tests
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
