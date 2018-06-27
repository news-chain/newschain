//
// Created by boy on 18-6-19.
//

#pragma once

#define NEWS_CHAIN_API_PLUGIN_NAME ("chain_api")

#include <app/application.hpp>
#include <news/plugins/chain_api/chain_api.hpp>


using namespace boost::program_options;

namespace news{
    namespace plugins{
        namespace chain_api_plugin{




            class chain_api_plugin : public news::app::plugin<chain_api_plugin>{
            public:
                chain_api_plugin();
                virtual  ~chain_api_plugin();

                NEWSAPP_PLUGIN_REQUIRES();

                static const std::string& name() { static std::string name = NEWS_CHAIN_API_PLUGIN_NAME; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::shared_ptr< chain_api > api;

            };











        }}}//news::plugins::chain