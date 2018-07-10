//
// Created by boy on 18-7-10.
//

#include <news/plugins/network_broadcast_plugin/network_broadcast_plugin.hpp>



namespace news{
    namespace plugins{
        namespace network_broadcast{

            network_broadcast_plugin::network_broadcast_plugin() {

            }

            network_broadcast_plugin::~network_broadcast_plugin() {

            }

            void network_broadcast_plugin::set_program_options(options_description &, options_description &cfg) {

            }

            void network_broadcast_plugin::plugin_initialize(const variables_map &options) {
                api = std::make_shared<network_broadcast_api>();
            }

            void network_broadcast_plugin::plugin_startup() {

            }

            void network_broadcast_plugin::plugin_shutdown() {

            }
        }
    }
}