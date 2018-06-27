//
// Created by boy on 18-6-25.
//



#include <news/plugins/database_api/database_api_plugin.hpp>


namespace news{
    namespace plugins{
        namespace database_api{



            database_api_plugin::database_api_plugin() {}

            database_api_plugin::~database_api_plugin() {

            }

            void database_api_plugin::set_program_options(options_description &, options_description &cfg) {

            }

            void database_api_plugin::plugin_initialize(const variables_map &options) {
                api = std::make_shared<database_api>();
            }

            void database_api_plugin::plugin_startup() {

            }

            void database_api_plugin::plugin_shutdown() {

            }


        }}}//news::plugins::database_api