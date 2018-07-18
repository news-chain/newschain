add_executable(test_transfer
        factory.cpp
        factory.hpp
        main.cpp)
target_link_libraries(test_transfer fc news_chain wsclient database_api)
