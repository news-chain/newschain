file(GLOB UNIT_TESTS "*.cpp")
add_executable( chainbase_test test.cpp  )
target_link_libraries( chainbase_test  chainbase ${PLATFORM_SPECIFIC_LIBS} -lpthread)



add_executable(testChainbaseRW testChainbaseRW.cpp)
target_link_libraries( testChainbaseRW  chainbase ${PLATFORM_SPECIFIC_LIBS} -lpthread)
