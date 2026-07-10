add_library(ring_buffer INTERFACE)

target_include_directories(
    ring_buffer
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)