

file(GLOB HEADERS "include/news/plugins/block_api_plugin/*.hpp")
add_library(block_api_plugin
        src/block_api_plugin.cpp
        src/block_api.cpp
        ${HEADERS})
target_link_libraries(block_api_plugin appbase fc chain_plugin json_rpc_plugin news_chain)
target_include_directories(block_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")