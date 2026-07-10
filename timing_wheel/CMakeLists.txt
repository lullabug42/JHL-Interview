add_library(timing_wheel STATIC)

target_include_directories(
    timing_wheel
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(
    timing_wheel
    PRIVATE
    timing_wheel.cpp
)