

add_library(news_protocol include/news/protocol/types.hpp)

target_link_libraries(news_protocol fc)
target_include_directories(news_protocol PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")