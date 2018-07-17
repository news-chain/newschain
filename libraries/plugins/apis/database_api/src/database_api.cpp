//
// Created by boy on 18-6-25.
//



#include <news/plugins/database_api/database_api.hpp>
#include <app/application.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <news/plugins/database_api/database_api_plugin.hpp>
#include <fc/crypto/hex.hpp>
#include <fc/io/raw.hpp>


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
                                (get_accounts)
                                (get_transactions_hex)
                                (get_transaction)
                        )


                news::chain::database &_db;
            };


            DEFINE_API_IMPL(database_api_impl, get_dynamic_global_property)
            {
                return _db.get_global_property_object();
            }


            DEFINE_API_IMPL(database_api_impl, get_accounts)
            {
                FC_ASSERT(args.accounts.size() <= API_LIMIT, "accounts.size > API_LIMIT");
                get_accounts_return ret;
                for(auto name : args.accounts){
                    const auto &ac = _db.get_account(name);
                    ret.accounts.emplace_back(account(ac));
                }
                return ret;
            }

            DEFINE_API_IMPL(database_api_impl, get_transactions_hex)
            {
                get_transactions_hex_return ret;
                ret.hash = std::move(fc::to_hex(fc::raw::pack_to_vector(args.trx)));
                return ret;
            }



            DEFINE_API_IMPL(database_api_impl, get_transaction)
            {
                get_transaction_return ret;
                return ret;
            }





            /////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////database_api/////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////


            database_api::database_api():my(new database_api_impl()) {
                JSON_RPC_REGISTER_API( NEWS_DATABASE_API_PLUGIN_NAME );
            }

            database_api::~database_api() {

            }

            DEFINE_READ_APIS(database_api,
                             (get_dynamic_global_property)
                                     (get_accounts)
                                     (get_transactions_hex)
                                     (get_transaction)
            )
        }
    }
}
