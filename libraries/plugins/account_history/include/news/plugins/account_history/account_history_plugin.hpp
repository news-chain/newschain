//
// Created by boy on 18-7-3.
//

#pragma once


#include <app/application.hpp>
#include <string>

#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <news/chain/operation_object.hpp>

#define NEWS_ACCOUNT_HISTORY_PLUGIN_NAME ("account_history_plugin")


namespace news{
    namespace plugins{
        namespace account_history_plugin{

            using namespace boost::program_options;


            namespace detail{
                class account_history_impl;
            }


            class account_history_plugin : public news::app::plugin<account_history_plugin>{
            public:
                account_history_plugin();
                virtual  ~account_history_plugin();

                NEWSAPP_PLUGIN_REQUIRES();

                static const std::string& name() { static std::string name = NEWS_ACCOUNT_HISTORY_PLUGIN_NAME; return name; }
                virtual void set_program_options(options_description&, options_description& cfg) override;


            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::unique_ptr<detail::account_history_impl> _my;
            };





}}}//news::plugins::account_history_plugin
