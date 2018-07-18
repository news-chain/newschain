
add_executable(verifyapithendocumentthem
        verifyapithendocumentthem.cpp
        vdafactory.cpp
        vdafactory.hpp)

target_link_libraries(verifyapithendocumentthem
        fc
        news_chain
        appbase
        wsclient
        database_api
        block_api_plugin
        chain_api_plugin
        network_broadcast
        account_history_api)
