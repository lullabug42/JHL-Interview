cmake_minimum_required(VERSION 3.16)

project(JHLInterview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ring_buffer)
add_subdirectory(timing_wheel)
add_subdirectory(order_book)

# tests
add_subdirectory(tests)

add_executable(run_tests)
target_sources(
    run_tests
    PRIVATE
    run_tests.cpp
)
target_link_libraries(
    run_tests
    PRIVATE
    test_ring_buffer
    test_timing_wheel
    test_order_book
)

# order_book
add_executable(run_order_book)

target_sources(
    run_order_book
    PRIVATE
    run_order_book.cpp
)
target_link_libraries(
    run_order_book
    PRIVATE
    order_book
)