//
// Created by boy on 18-6-19.
//

#include <news/plugins/chain_api/chain_api_plugin.hpp>



namespace news{
    namespace plugins{
        namespace chain_api_plugin{

            void chain_api_plugin::plugin_shutdown() {

            }

            chain_api_plugin::chain_api_plugin() {

            }

            chain_api_plugin::~chain_api_plugin() {

            }

            void chain_api_plugin::set_program_options(options_description &, options_description &cfg) {

            }

            void chain_api_plugin::plugin_initialize(const variables_map &options) {
                api = std::make_shared<chain_api>();
            }

            void chain_api_plugin::plugin_startup() {

            }
        }}}//news::plugins::chain_api