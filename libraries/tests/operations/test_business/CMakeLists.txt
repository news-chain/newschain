


add_executable(test_business
        factory.cpp
        factory.hpp
        main.cpp)
target_link_libraries(fc news_chain wsclient)
