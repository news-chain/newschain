

project(NEWS)


add_executable(NEWS main.cpp)
target_link_libraries(NEWS ${Boost_LIBRARIES}
        appbase
        webserver_plugin
        producer_plugin
        chain_plugin
        fc
        news_protocol
        block_api_plugin
        chain_api_plugin
        database_api)
#target_include_directories(chainMain ${CMAKE_CURRENT_SOURCE_DIR})