

project(chainMain)


add_executable(chainMain main.cpp)
target_link_libraries(chainMain ${Boost_LIBRARIES} appbase)
#target_include_directories(chainMain ${})