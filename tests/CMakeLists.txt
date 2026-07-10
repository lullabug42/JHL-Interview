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