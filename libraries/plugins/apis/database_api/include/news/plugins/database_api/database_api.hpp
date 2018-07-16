//
// Created by boy on 18-6-25.
//

#pragma once

#include <news/plugins/json_rpc/utility.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/database_api/database_api_args.hpp>

#include <news/plugins/database_api/database_api.hpp>



namespace news{
    namespace plugins{
        namespace database_api{


            class database_api_impl;

            class database_api{
            public:

                database_api();
                ~database_api();

                DECLARE_API(
                        (get_dynamic_global_property)
                                (get_accounts)
                                (get_transactions_hex)
                                (get_transaction)
                        )


            private:
                std::unique_ptr<database_api_impl> my;
            };
        }
    }
}