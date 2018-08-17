


add_executable(test_create_account
        src/factory.cpp
        src/types.cpp
        src/application.cpp
        src/record.cpp

        include/test/factory.hpp
        include/test/types.hpp
        include/test/application.hpp
        include/test/record.hpp

        main.cpp)
target_link_libraries(test_create_account fc news_chain wsclient)

include_directories(include/)
