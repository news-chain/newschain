//
// Created by boy on 18-6-20.
//


#pragma once

#include <news/chain/block_header.hpp>
#include <news/chain/block.hpp>


namespace news{
    namespace plugins{
        namespace block_api_plugin{

            /* get_block_header */

            struct get_block_header_args
            {
                uint32_t block_num = 0;
            };

            struct get_block_header_return
            {
                fc::optional< news::chain::block_header > header;
            };

            /* get_block */
            struct get_block_args
            {
                uint32_t block_num = 0;
            };

            struct get_block_return
            {
                fc::optional< news::chain::signed_block > block;
            };
//




            /* fetch_chunk */
            struct fetch_chunk_args
            {
                uint32_t chunk_num = 0;
            };
            struct fetch_chunk_return
            {
                fc::optional< news::chain::signed_block > chunk;
            };



        }
    }
}

FC_REFLECT(news::plugins::block_api_plugin::get_block_header_args, (block_num))
FC_REFLECT(news::plugins::block_api_plugin::get_block_header_return, (header))
FC_REFLECT(news::plugins::block_api_plugin::get_block_args, (block_num))
FC_REFLECT(news::plugins::block_api_plugin::get_block_return, (block))



FC_REFLECT(news::plugins::block_api_plugin::fetch_chunk_args, (chunk_num))
FC_REFLECT(news::plugins::block_api_plugin::fetch_chunk_return, (chunk))
