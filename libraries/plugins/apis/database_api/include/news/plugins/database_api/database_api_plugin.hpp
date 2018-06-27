//
// Created by boy on 18-6-25.
//

#pragma once



#include <news/plugins/database_api/database_api.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>


#define NEWS_DATABASE_API_PLUGIN_NAME   ("database_api")

using namespace boost::program_options;
namespace news{
    namespace plugins{
        namespace database_api{

            class database_api_plugin : public news::app::plugin<database_api_plugin>{
            public:
                database_api_plugin();
                virtual  ~database_api_plugin();

                NEWSAPP_PLUGIN_REQUIRES(
                        (news::plugins::json_rpc::json_rpc_plugin)
                                (news::plugins::chain_plugin::chain_plugin))

                static const std::string& name() { static std::string name = NEWS_DATABASE_API_PLUGIN_NAME; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::shared_ptr< database_api > api;

            };






        }}}//news::plugins::database_api