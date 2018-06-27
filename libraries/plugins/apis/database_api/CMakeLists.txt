

add_library(database_api
            src/database_api.cpp
            src/databae_api_plugin.cpp
            include/news/plugins/database_api/database_api.hpp
            include/news/plugins/database_api/database_api_plugin.hpp
            include/news/plugins/database_api/database_api_args.hpp)


target_link_libraries(database_api
        appbase
        news_chain
        json_rpc_plugin
        chain_plugin
        )

target_include_directories(database_api PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")