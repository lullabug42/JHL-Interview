add_library(order_book STATIC)

target_include_directories(
    order_book
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(
    order_book
    PRIVATE
    order_book.cpp
)