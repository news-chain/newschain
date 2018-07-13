file(GLOB BASE_HEADDERS "include/news/base/*.hpp")
add_library(news_protocol
        src/base/types.cpp
        src/base/operation.cpp
        src/base/version.cpp
        src/base/asset.cpp
        src/base/key_conversion.cpp
        ${BASE_HEADDERS}
        )
target_link_libraries(news_protocol fc chainbase)
target_include_directories(news_protocol PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")






file(GLOB CHAIN_HEADERS "include/news/chain/*.hpp")
add_library(news_chain
            src/chain/database.cpp
            src/chain/block.cpp
            src/chain/block_header.cpp
            src/chain/block_log.cpp
            src/chain/transaction.cpp
            src/chain/fork_database.cpp
            src/chain/news_eveluator.cpp
            src/chain/transaction_object.cpp
            ${CHAIN_HEADERS}
        )

target_link_libraries(news_chain fc news_protocol chainbase appbase)
target_include_directories(news_chain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")




