//
// Created by boy on 18-7-16.
//
#pragma once


#include <news/plugins/account_history_api/account_history_api_args.hpp>
#include <fc/unique_ptr.hpp>

#include <news/plugins/json_rpc/utility.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
namespace news{
    namespace plugins{
        namespace account_history_api{



            namespace detail{
                class account_history_api_impl;
            }

            class account_history_api{
            public:
                account_history_api();
                ~account_history_api();

                DECLARE_API(
                        (get_transaction)
                        (get_account_history)
                )


            private:
                std::unique_ptr<detail::account_history_api_impl> my;
            };




        }
    }
}