

project(baseChain)
cmake_minimum_required(VERSION 3.0)




OPTION( CHAINBASE_CHECK_LOCKING "Check locks in chainbase (ON or OFF)" ON )
MESSAGE( STATUS "CHAINBASE_CHECK_LOCKING: ${CHAINBASE_CHECK_LOCKING}" )
if( CHAINBASE_CHECK_LOCKING )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCHAINBASE_CHECK_LOCKING" )
    SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCHAINBASE_CHECK_LOCKING" )
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.4)
        message(FATAL_ERROR "GCC version must DB at least 5.4!")
    endif()
endif()



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wno-unused-variable -pthread")







add_subdirectory(libraries)