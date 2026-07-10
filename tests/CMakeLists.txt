# ring_buffer
add_library(test_ring_buffer STATIC)

target_include_directories(
    test_ring_buffer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
    test_ring_buffer
    PRIVATE
    ${CMAKE_SOURCE_DIR}/ring_buffer
)

target_sources(
    test_ring_buffer
    PRIVATE
    test_ring_buffer.cpp
)

target_link_libraries(
    test_ring_buffer
    PRIVATE
    ring_buffer
)

# timing_wheel
add_library(test_timing_wheel STATIC)

target_include_directories(
    test_timing_wheel
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
    test_timing_wheel
    PRIVATE
    ${CMAKE_SOURCE_DIR}/timing_wheel
)

target_sources(
    test_timing_wheel
    PRIVATE
    test_timing_wheel.cpp
)

target_link_libraries(
    test_timing_wheel
    PRIVATE
    timing_wheel
)