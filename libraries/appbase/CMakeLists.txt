



SET(BOOST_COMPONENTS)
LIST(APPEND BOOST_COMPONENTS thread
        date_time
        system
        filesystem
        chrono
        program_options
        unit_test_framework
        locale)
FIND_PACKAGE(Boost 1.58 REQUIRED COMPONENTS ${BOOST_COMPONENTS})



add_library(appbase src/application.cpp include/app/application.hpp)
target_link_libraries(appbase ${Boost_LIBRARIES})
target_include_directories(appbase PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIR})