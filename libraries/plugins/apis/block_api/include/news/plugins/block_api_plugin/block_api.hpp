//
// Created by boy on 18-6-20.
//

#pragma once

#include <memory>


#include <news/plugins/json_rpc/utility.hpp>
#include <news/plugins/block_api_plugin/block_api_args.hpp>
#include <news/chain/database.hpp>

namespace news{
    namespace plugins{
        namespace block_api_plugin{


            class block_api_impl;



            class block_api{
            public:
                block_api();
                ~block_api();

                DECLARE_API(

                /////////////////////////////
                // Blocks and transactions //
                /////////////////////////////

                /**
                * @brief Retrieve a block header
                * @param block_num Height of the block whose header should be returned
                * @return header of the referenced block, or null if no matching block was found
                */
                (get_block_header)

                /**
                * @brief Retrieve a full, signed block
                * @param block_num Height of the block to be returned
                * @return the referenced block, or null if no matching block was found
                */
                (get_block)

                /* do a little practice by Oijen*/
                (fetch_chunk)

                )



            private:
                std::unique_ptr<  block_api_impl > my;
            };



        }}}//news::plugins::block_api_plugin
