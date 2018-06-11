
file(GLOB HEADERS "include/news/chain/*.hpp")

add_library(news_chain
            src/database.cpp
            src/block_log.cpp
            ${HEADERS}
        src/block.cpp include/news/chain/block.hpp)

target_link_libraries(news_chain fc news_protocol)
target_include_directories(news_chain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")