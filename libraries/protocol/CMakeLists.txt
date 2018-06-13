FILE(GLOB HEADERS "include/news/protocol/*.hpp")



add_library(news_protocol
        src/types.cpp
        ${HEADERS})

target_link_libraries(news_protocol fc )
target_include_directories(news_protocol PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")