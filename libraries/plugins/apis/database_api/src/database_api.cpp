//
// Created by boy on 18-6-25.
//



#include <news/plugins/database_api/database_api.hpp>
#include <app/application.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <news/plugins/database_api/database_api_plugin.hpp>



namespace news{
    namespace plugins{
        namespace database_api{


            class database_api_impl{
            public:
                database_api_impl():_db(news::app::application::getInstance().get_plugin< news::plugins::chain_plugin::chain_plugin>().get_database() ){

                }
                ~database_api_impl(){}

                DECLARE_API_IMPL(
                        (get_dynamic_global_property)
                        )


                news::chain::database &_db;
            };


            DEFINE_API_IMPL(database_api_impl, get_dynamic_global_property)
            {
                return _db.get_global_property_object();
            }







            DEFINE_READ_APIS(database_api,
                             (get_dynamic_global_property)
            )

            database_api::database_api():my(new database_api_impl()) {
                JSON_RPC_REGISTER_API( NEWS_DATABASE_API_PLUGIN_NAME );
            }

            database_api::~database_api() {

            }
        }
    }
}
