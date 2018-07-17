

project(baseChain)
cmake_minimum_required(VERSION 3.0)
if(MSVC)
#set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
set(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
endif()



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



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")







add_subdirectory(libraries)
add_subdirectory(programs)