

project(chainMain)


add_executable(chainMain main.cpp)
target_link_libraries(chainMain ${Boost_LIBRARIES} appbase webserver_plugin producer_plugin chain_plugin fc news_protocol)
#target_include_directories(chainMain ${CMAKE_CURRENT_SOURCE_DIR})