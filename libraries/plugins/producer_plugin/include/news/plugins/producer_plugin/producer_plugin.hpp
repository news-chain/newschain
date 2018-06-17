//
// Created by boy on 18-6-12.
//

#pragma once




#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include <boost/bind.hpp>

#include <memory>

#include <fc/log/logger.hpp>
#include <fc/time.hpp>
#include <fc/variant_object.hpp>

#include <app/application.hpp>
#include <news/base/config.hpp>
#include <news/base/types.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>


#define NEWS_PRODUCER_PLUGIN_NAME ("producer_plugin")

using namespace boost::program_options;
namespace news{

    namespace block_production_condition {
        enum block_production_condition_enum
        {
            produced = 0,
            not_synced = 1,
            not_my_turn = 2,
            not_time_yet = 3,
            no_private_key = 4,
            low_participation = 5,
            lag = 6,
            consecutive = 7,
            exception_producing_block = 8,
            wait_for_genesis = 9
        };
    }


    namespace plugins{

        namespace producer_plugin{
            namespace detail{
                class producer_plugin_impl;
            }


            class producer_plugin : public news::app::plugin<producer_plugin>{
            public:
                producer_plugin();
                virtual  ~producer_plugin();
                NEWSAPP_PLUGIN_REQUIRES();

                static const std::string &name(){static std::string name = NEWS_PRODUCER_PLUGIN_NAME; return name;}

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;

            private:
                std::unique_ptr< detail::producer_plugin_impl > _my;
            };
        }




    }//namespace plugins
}//namespace news