add_library(account_his_plugin
        src/account_history_plugin.cpp
        include/news/plugins/account_history/account_history_plugin.hpp)
target_link_libraries(account_his_plugin appbase fc news_chain)
target_include_directories(account_his_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")