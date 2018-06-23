


add_library(chain_api_plugin
        src/chain_api_plugin.cpp
        src/chain_api.cpp
        include/news/plugins/chain_api/chain_api_plugin.hpp
        include/news/plugins/chain_api/chain_api.hpp)
target_link_libraries(chain_api_plugin
        appbase
        fc
        news_chain
        json_rpc_plugin
        chain_plugin)
target_include_directories(chain_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")