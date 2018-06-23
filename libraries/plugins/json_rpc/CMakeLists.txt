file(GLOB HEADERS "include/news/plugins/json_rpc/*.hpp")
add_library(json_rpc_plugin
        src/json_rpc_plugin.cpp
        ${HEADERS})

target_link_libraries(json_rpc_plugin appbase fc chainbase)
target_include_directories(json_rpc_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")