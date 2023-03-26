cmake_minimum_required(VERSION 3.22)
project(RunTest C CXX)
set(CMAKE_CXX_STANDARD 14)
include_directories(RunTest PRIVATE .. ../rtos)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(RunTest MemPoolTest.cpp)

target_link_libraries(
        RunTest
        GTest::gtest_main
)