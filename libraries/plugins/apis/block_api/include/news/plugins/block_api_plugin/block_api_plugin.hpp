//
// Created by boy on 18-6-20.
//

#pragma once

#include <app/application.hpp>

#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <news/plugins/block_api_plugin/block_api.hpp>

#define NEWS_BLOCK_API_PLUGIN_NAME      ("block_api")

using namespace boost::program_options;

namespace news{
    namespace plugins{
        namespace block_api_plugin{


            class block_api_plugin : public news::app::plugin< block_api_plugin >
            {
            public:
                block_api_plugin();
                virtual ~block_api_plugin();

                NEWSAPP_PLUGIN_REQUIRES(
                (news::plugins::json_rpc::json_rpc_plugin)
                        (news::plugins::chain_plugin::chain_plugin)
                )

                static const std::string& name() { static std::string name = NEWS_BLOCK_API_PLUGIN_NAME; return name; }

                virtual void set_program_options(
                        options_description& cli,
                        options_description& cfg ) override;
                void plugin_initialize( const variables_map& options ) override;
                void plugin_startup() override;
                void plugin_shutdown() override;

                std::shared_ptr< class block_api > api;
            };






}}}//news::plugins::block_api_plugin