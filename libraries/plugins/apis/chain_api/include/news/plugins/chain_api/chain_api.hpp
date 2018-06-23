//
// Created by boy on 18-6-20.
//

#pragma once

#include <news/chain/block.hpp>
#include <news/chain/transaction.hpp>
#include <news/plugins/json_rpc/utility.hpp>
#include <fc/optional.hpp>


#include <news/plugins/chain_plugin/chain_plugin.hpp>


namespace news{
    namespace plugins{
        namespace chain_api_plugin{

            using namespace std;
            using namespace fc;


            namespace detail{
                class chain_api_impl;
            }

            struct push_block_args
            {
                news::chain::signed_block       block;
                bool                            currently_syncing = false;
            };


            struct push_block_return
            {
                bool              success;
                optional<string>  error;
            };

            typedef news::chain::signed_transaction push_transaction_args;

            struct push_transaction_return
            {
                bool              success;
                optional<string>  error;
            };


            class chain_api
            {
            public:
                chain_api();
                ~chain_api();

                DECLARE_API(
                (push_block)
                        (push_transaction)
                )

            private:
                std::unique_ptr< detail::chain_api_impl > my;
            };





        }}}//news::plugins::chain

FC_REFLECT(news::plugins::chain_api_plugin::push_block_args, (block)(currently_syncing))
FC_REFLECT(news::plugins::chain_api_plugin::push_block_return, (success)(error))
FC_REFLECT(news::plugins::chain_api_plugin::push_transaction_return, (success)(error))