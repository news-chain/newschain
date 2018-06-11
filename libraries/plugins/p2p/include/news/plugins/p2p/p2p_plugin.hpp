//
// Created by boy on 18-6-11.
//

#ifndef BASECHAIN_P2P_PLUGIN_HPP
#define BASECHAIN_P2P_PLUGIN_HPP


#include <app/application.hpp>

#define  NEWS_P2P_PLUGIN_NAME ("p2p_plugin")


using namespace boost::program_options;

namespace news{
    namespace plugins{
        namespace p2p{

            namespace detail{
                class p2p_plugin_impl;
            }


            class p2p_plugin : public news::app::plugin<p2p_plugin>{
            public:
                p2p_plugin();
                virtual  ~p2p_plugin();

                NEWSAPP_PLUGIN_REQUIRES();

                static const std::string& name() { static std::string name = NEWS_P2P_PLUGIN_NAME; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;

            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;
            private:
                std::unique_ptr< detail::p2p_plugin_impl > _my;

            };
        }//namespace p2p
    }//namespace plugins
}//namespace news






#endif //BASECHAIN_P2P_PLUGIN_HPP
