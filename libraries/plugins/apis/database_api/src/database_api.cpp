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
                                (get_account_public_key)
                                (get_transactions_hex)
                                (get_transaction)
                                (get_comment_by_id)
                                (get_comment_by_permlink)
								(get_comment_vote_by_id)
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


            DEFINE_API_IMPL(database_api_impl, get_comment_by_id)
            {
                get_comment_by_id_return ret;
                ret.success = false;
                try {
                    const auto &com = _db.get(comment_object::id_type(args.id));
                    ret.success = true;
                    ret.data = com;
                    return ret;
                }catch (...){
                    return ret;
                }
            }

            DEFINE_API_IMPL(database_api_impl,  get_comment_by_permlink)
            {
                get_comment_by_permlink_return ret;
                ret.success = false;
                const auto &itr = _db.get_index<comment_object_index>().indices().get<by_permlink>();
                const auto &com = itr.find(boost::make_tuple(args.author, args.permlink));
                if(com != itr.end()){
                    ret.data = comment(*com);
                }

                return ret;
            }


            DEFINE_API_IMPL(database_api_impl, get_account_public_key)
            {
                get_account_public_key_return ret;
                ret.success = false;
                const auto &itr = _db.get<account_authority_object, by_name>(args.name);
                ret.data = itr;
                ret.success = true;

                return ret;
            }


			DEFINE_API_IMPL(database_api_impl, get_comment_vote_by_id)
			{
				get_comment_vote_by_id_return ret;
				ret.success = false; 
				const auto &itr = _db.get_index<comment_vote_object_index>().indices().get<by_comment_id>(); 
				auto begin=itr.lower_bound(args.id);
				auto end=itr.upper_bound(args.id);
				if (begin != end)
				{
					ret.author = begin->author;
					ret.permlink = begin->permlink.c_str();
				}
				for (; begin != end; begin++)
					ret.data.emplace_back(*begin);
				ret.success = true;
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
                                     (get_account_public_key)
                                     (get_transactions_hex)
                                     (get_transaction)
                                     (get_comment_by_id)
                                     (get_comment_by_permlink)
									 (get_comment_vote_by_id)
                            )
        }
    }
}
