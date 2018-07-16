//
// Created by boy on 18-7-16.
//



#include <news/plugins/account_history_api/account_history_api.hpp>
#include <news/chain/database.hpp>
#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <news/plugins/account_history_api/account_history_api_plugin.hpp>
#include <news/plugins/account_history/account_history_plugin.hpp>

namespace news{
    namespace plugins{
        namespace account_history_api{

            namespace detail{
                class account_history_api_impl{
                public:
                    account_history_api_impl():_db(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>().get_database()){}

                    DECLARE_API_IMPL(
                            (get_transaction)
                            (get_account_history)
                    )


                private:
                    news::chain::database &_db;
                };



                DEFINE_API_IMPL(account_history_api_impl, get_transaction)
                {
                    get_transaction_return ret;
                    const auto &his_index = _db.get_index<news::chain::operation_obj_index, news::chain::by_trx_id>();

                    auto itr = his_index.find(args.trx_id);
                    FC_ASSERT(itr != his_index.end(), "unkown transaction ${id}", ("id", args.trx_id));
                    FC_ASSERT(itr->trx_id == args.trx_id, "unkown transaction ${id}", ("id", args.trx_id));

                    auto block = _db.fetch_block_by_number(itr->block);
                    FC_ASSERT(block.valid());
                    FC_ASSERT(block->transactions.size() > itr->trx_in_block);

                    ret.trx = block->transactions[itr->trx_in_block];
                    ret.trx.block_num = itr->block;
                    ret.trx.transaction_num = itr->trx_in_block;

                    return ret;
                }


                DEFINE_API_IMPL(account_history_api_impl, get_account_history)
                {
                    FC_ASSERT(args.limit <= API_LIMIT, "limit <= API_LIMIT");
                    FC_ASSERT(args.start >= args.limit, "start must be greater than limit");


                    get_account_history_return ret;

                    const auto &idx = _db.get_index<news::chain::account_history_obj_index, news::chain::by_account>();
                    auto itr = idx.lower_bound(boost::make_tuple(args.name, args.start));
                    uint32_t num = 0;
                    for(;itr != idx.end(); itr++){
                        if(num >= args.limit){
                            break;
                        }
                        if(itr->name != args.name){
                            break;
                        }
                        ret.trxs[itr->sequence] = _db.get(itr->op_id);
                        num++;
                    }

                    return ret;
                }

            }


            account_history_api::account_history_api():my(new detail::account_history_api_impl()) {
                if(news::app::abstract_plugin::state::started == news::app::application::getInstance().get_plugin<news::plugins::account_history_plugin::account_history_plugin>().get_state()){
                    JSON_RPC_REGISTER_API(ACCOUNT_HISTORY_API_NAME_PLUGIN)
                }else{
                    elog("not started account_history_api, if want use, please start account_history_plugin.");
                }

            }

            account_history_api::~account_history_api() {

            }

            DEFINE_LOCKLESS_APIS( account_history_api,
            (get_transaction)
            (get_account_history)
            )


        }
    }
}

