//
// Created by boy on 18-7-10.
//

#pragma once


#include <app/application.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/p2p/p2p_plugin.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <app/plugin.hpp>


#include <news/plugins/network_broadcast_plugin/network_broadcast_api.hpp>


#define NEWS_NETWORK_BROADCAST_PLUGIN   ("network_broadcast_api")

namespace news{
    namespace plugins{
        namespace network_broadcast{

            using namespace boost::program_options;

            class network_broadcast_plugin : public news::app::plugin<network_broadcast_plugin>{
            public:
                network_broadcast_plugin();
                virtual  ~network_broadcast_plugin();

                NEWSAPP_PLUGIN_REQUIRES(
                (news::plugins::json_rpc::json_rpc_plugin)
                        (news::plugins::chain_plugin::chain_plugin)
                                (news::plugins::p2p::p2p_plugin))

                static const std::string& name() { static std::string name = NEWS_NETWORK_BROADCAST_PLUGIN; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::shared_ptr< network_broadcast_api > api;

            };

        }
    }
}