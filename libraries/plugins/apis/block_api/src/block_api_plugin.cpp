//
// Created by boy on 18-6-20.
//


#include <news/plugins/block_api_plugin/block_api_plugin.hpp>


namespace news{
    namespace plugins{
        namespace block_api_plugin{


            block_api_plugin::block_api_plugin() {

            }

            block_api_plugin::~block_api_plugin() {

            }

            void block_api_plugin::set_program_options(options_description &cli, options_description &cfg) {

            }

            void block_api_plugin::plugin_initialize(const variables_map &options) {
                api = std::make_shared<block_api>();
            }

            void block_api_plugin::plugin_startup() {

            }

            void block_api_plugin::plugin_shutdown() {

            }





















            
        }}}//news::plugin::block_api_plugin
