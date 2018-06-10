


SET(BOOST_COMPONENTS)
LIST(APPEND BOOST_COMPONENTS thread
        date_time
        system
        filesystem
        chrono
        unit_test_framework
        locale)

SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )


add_executable(ripemd160Test test..cpp)
target_link_libraries(ripemd160Test chainbase fc ${PLATFORM_SPECIFIC_LIBS})