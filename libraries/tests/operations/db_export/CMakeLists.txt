 

add_executable(db_export       
        main.cpp)
link_directories("/usr/lib/x86_64-linux-gnu")
target_link_libraries(db_export  mysqlclient news_chain fc )
