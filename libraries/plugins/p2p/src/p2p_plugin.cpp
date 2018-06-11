//
// Created by boy on 18-6-11.
//

#include "news/plugins/p2p/p2p_plugin.hpp"

#include <graphene/net/node.hpp>
namespace news{
    namespace plugins{
        namespace p2p{
            namespace detail{
                class p2p_plugin_impl : public graphene::net::node_delegate{
                public:
                    p2p_plugin_impl(){

                    }

                    ~p2p_plugin_impl(){

                    }



                };
            }//namespace news::plugins::p2p::detail



            ///////////////////////////////////////////////
            /////////////////////p2p_plugin ///////////////
            ///////////////////////////////////////////////


            p2p_plugin::p2p_plugin():_my(new detail::p2p_plugin_impl()) {

            }

            p2p_plugin::~p2p_plugin() {

            }

            void p2p_plugin::set_program_options(options_description &cli, options_description &cfg) {
                cfg.add_options()
                        ("p2p-endpoing", bpo::value<std::string>()->implicit_value("127.0.0.1:6002"), "p2p server port.")
                        ("p2p-max-connections", bpo::value<uint32_t >()->default_value(24), "Maxmimum number of incoming connections on P2P endpoint.")
                        ("seed-node", bpo::value< std::vector<std::string> >()->composing(), "The IP address and port of a remote peer to sync with. (split by space)")
                        ("p2p-parameters", bpo::value<std::string>(), "p2p parameters")
                        ("p2p-fore-validate", bpo::bool_switch()->default_value(false), "Force validation of all transactions");

            }

            void p2p_plugin::plugin_initialize(const variables_map &options) {

            }

            void p2p_plugin::plugin_startup() {

            }

            void p2p_plugin::plugin_shutdown() {

            }



        }//namespace p2p
    }//namespace plugins
}//namespace news