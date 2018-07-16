

add_library(account_history_api
        src/account_history_api.cpp
        src/account_history_api_plugin.cpp
        include/news/plugins/account_history_api/account_history_api.hpp
        include/news/plugins/account_history_api/account_history_api_args.hpp
        include/news/plugins/account_history_api/account_history_api_plugin.hpp)


target_link_libraries(account_history_api
        appbase
        news_chain
        json_rpc_plugin
        chain_plugin
        )

target_include_directories(account_history_api PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
