
add_library(wsclient
        wsClient.cpp
        wsClient.hpp)

target_link_libraries(wsclient fc)
target_include_directories(wsclient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")