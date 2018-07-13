
add_library(network_broadcast
        src/network_broadcast_plugin.cpp
        src/network_broadcast_api.cpp
        include/news/plugins/network_broadcast_plugin/network_broadcast_plugin.hpp
        include/news/plugins/network_broadcast_plugin/network_broadcast_api.hpp)


target_link_libraries(network_broadcast
        appbase
        news_chain
        json_rpc_plugin
        chain_plugin
        p2p_plugin
        )

target_include_directories(network_broadcast PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")