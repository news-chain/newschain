


file(GLOB HEADERS "include/news/plugins/webserver/*.hpp")
add_library(webserver_plugin
        src/webserver_plugin.cpp
        ${HEADERS})

target_link_libraries(webserver_plugin appbase fc )
target_include_directories(webserver_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
