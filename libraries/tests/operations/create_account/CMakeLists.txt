


add_executable(test_create_account
        factory.cpp
        factory.hpp
        main.cpp)
target_link_libraries(test_create_account fc news_chain wsclient)
