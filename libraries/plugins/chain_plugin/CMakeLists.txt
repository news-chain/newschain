

file(GLOB HEADERS "include/news/plugins/chain_plugin/*.hpp")
add_library(chain_plugin
        src/chain_plugin.cpp
        ${HEADERS})
target_link_libraries(chain_plugin appbase fc news_chain)
target_include_directories(chain_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")