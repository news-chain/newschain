//
// Created by boy on 18-7-16.
//

#pragma once



#include <app/application.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <app/plugin.hpp>
#include <news/plugins/account_history_api/account_history_api.hpp>
#include <news/plugins/account_history/account_history_plugin.hpp>
#define ACCOUNT_HISTORY_API_NAME_PLUGIN         ("account_history_api")


namespace news{
    namespace plugins{
        namespace account_history_api{

            using namespace boost::program_options;

            class account_history_api_plugin : public news::app::plugin<account_history_api_plugin>{
            public:
                account_history_api_plugin();
                virtual  ~account_history_api_plugin();

                NEWSAPP_PLUGIN_REQUIRES(
                (news::plugins::json_rpc::json_rpc_plugin)
                        (news::plugins::chain_plugin::chain_plugin)
                        (news::plugins::account_history_plugin::account_history_plugin)
                        )

                static const std::string& name() { static std::string name = ACCOUNT_HISTORY_API_NAME_PLUGIN; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::shared_ptr< account_history_api > api;

            };


        }
    }
}