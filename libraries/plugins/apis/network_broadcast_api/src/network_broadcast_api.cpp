//
// Created by boy on 18-7-10.
//

#include <news/plugins/network_broadcast_plugin/network_broadcast_api.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/network_broadcast_plugin/network_broadcast_plugin.hpp>


namespace news{
    namespace plugins{
        namespace network_broadcast{

            namespace detail{
                class network_broadcast_impl{
                public:
                    network_broadcast_impl():_p2p(news::app::application::getInstance().get_plugin<news::plugins::p2p::p2p_plugin>()),_chain(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>()){}

                    DECLARE_API_IMPL(
                            (broadcast_transaction)
                    )


                    news::plugins::p2p::p2p_plugin      &_p2p;
                    news::plugins::chain_plugin::chain_plugin &_chain;

                };




                DEFINE_API_IMPL(network_broadcast_impl, broadcast_transaction)
                {
                    _p2p.broadcast_transaction(args.trx);
//                    _chain.accept_transaction(args.trx);
                    return broadcast_transaction_return();
                }

            }



            network_broadcast_api::network_broadcast_api():my(new detail::network_broadcast_impl()) {
                JSON_RPC_REGISTER_API(NEWS_NETWORK_BROADCAST_PLUGIN)
            }

            network_broadcast_api::~network_broadcast_api() {

            }


            DEFINE_LOCKLESS_APIS( network_broadcast_api,
                                  (broadcast_transaction)
            )


        }
    }
}