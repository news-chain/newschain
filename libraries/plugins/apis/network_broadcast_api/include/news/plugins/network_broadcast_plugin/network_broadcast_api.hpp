//
// Created by boy on 18-7-10.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/plugins/json_rpc/utility.hpp>

namespace news{
    namespace plugins{
        namespace network_broadcast{


            using namespace news::chain;
            using namespace news::plugins::json_rpc;

            namespace  detail{
                class network_broadcast_impl;
            };


            typedef void_type   broadcast_transaction_return;

            struct broadcast_transaction_args{
                signed_transaction      trx;
            };


            class network_broadcast_api{
            public:
                network_broadcast_api();
                ~network_broadcast_api();


                DECLARE_API(
                        (broadcast_transaction)
                        )
            private:
                std::unique_ptr<detail::network_broadcast_impl> my;

            };


        }
    }
}


FC_REFLECT(news::plugins::network_broadcast::broadcast_transaction_args, (trx))