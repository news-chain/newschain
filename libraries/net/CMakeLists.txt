file(GLOB HEADERS "include/graphene/net/*.hpp")

set(SOURCES node.cpp
            stcp_socket.cpp
            core_messages.cpp
            peer_database.cpp
            peer_connection.cpp
            message_oriented_connection.cpp)

add_library( net ${SOURCES} ${HEADERS} )

target_link_libraries( net
   PUBLIC  fc )
 #PUBLIC steem_protocol statsd_plugin fc )
target_include_directories( net
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"  "${CMAKE_CURRENT_BINARY_DIR}/include" "${CMAKE_SOURCE_DIR}/libraries/chain/include" "${CMAKE_SOURCE_DIR}/libraries/chainbase/include"
       # "${CMAKE_SOURCE_DIR}/libraries/plugins/chain_plugin/include"


)

if(MSVC)
  set_source_files_properties( node.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

if (USE_PCH)
  set_target_properties(graphene_net PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
  cotire(net)
endif(USE_PCH)

if( CLANG_TIDY_EXE )
   set_target_properties(
           net PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
   )
endif( CLANG_TIDY_EXE )

install( TARGETS
        net

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/graphene/net" )
