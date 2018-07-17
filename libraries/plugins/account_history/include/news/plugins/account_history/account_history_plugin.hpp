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
            using namespace news::base;

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


            struct get_operation_names_visitor{
                flat_set<account_name>  &_names;
                get_operation_names_visitor(flat_set<account_name> &names):_names(names){}
                typedef void result_type;


                template <typename T>
                void operator()(const T&op){

                }

                void operator()(const create_account_operation &op){
                    _names.insert(op.name);
                    _names.insert(op.creator);
                }

                void operator()(const transfer_operation &op){
                    _names.insert(op.from);
                    _names.insert(op.to);
                }

                void operator()(const transfers_operation &op){
                    _names.insert(op.from);
                    for(auto &n : op.to_names){
                        _names.insert(n.first);
                    }
                }
            };





}}}//news::plugins::account_history_plugin
