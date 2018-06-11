
file(GLOB HEADERS "include/news/chain/*.hpp")

add_library(news_chain
            src/database.cpp
            src/block_log.cpp
            src/block.cpp
            src/block_header.cpp
            ${HEADERS}
        )

target_link_libraries(news_chain fc news_protocol)
target_include_directories(news_chain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")