


add_executable(test_create_account
        factory.cpp
        types.cpp

        factory.hpp
        types.hpp

        main.cpp)
target_link_libraries(test_create_account fc news_chain wsclient)
