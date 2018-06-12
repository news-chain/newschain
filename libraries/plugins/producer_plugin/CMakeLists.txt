


file(GLOB HEADERS "include/news/plugins/producer_plugin/*.hpp")

add_library(producer_plugin
            src/producer_plugin.cpp
            ${HEADERS})

target_link_libraries(producer_plugin appbase fc news_protocol)
target_include_directories(producer_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")