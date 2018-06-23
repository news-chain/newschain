//
// Created by boy on 18-6-20.
//

#include <news/plugins/chain_api/chain_api.hpp>
#include <news/plugins/chain_api/chain_api_plugin.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/base/types.hpp>
namespace news{
    namespace plugins{
        namespace chain_api_plugin{


            namespace detail{
                class chain_api_impl{
                public:
                    chain_api_impl():_chain_plugin(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>() ){}


                    DECLARE_API_IMPL(
                            (push_block)
                                    (push_transaction) )

                private:
                    news::plugins::chain_plugin::chain_plugin &_chain_plugin;
                };


                DEFINE_API_IMPL(chain_api_impl, push_block)
                {
                    push_block_return result;
                    result.error = "{\"code\":\"TODO push_block\"}";
                    return  result;
                }

                DEFINE_API_IMPL(chain_api_impl, push_transaction)
                {
                    push_transaction_return result;


                    try {
                        news::base::private_key_type private_key_type = news::base::private_key_type::generate();

                        news::chain::signed_transaction trx;
                        news::base::create_account_operation op;
                        op.name = fc::time_point::now().sec_since_epoch() % 100000;
                        op.creator = 1;
                        op.public_key = private_key_type.get_public_key();

                        trx.expiration = fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 10);
                        trx.operations.push_back(op);
                        trx.sign(private_key_type, NEWS_CHAIN_ID);
                        _chain_plugin.accept_transaction(trx);
                    }catch (const fc::exception &e){
                        result.error = e.to_detail_string();
                        result.success = false;
                    }catch (const std::exception &e){
                        result.error = e.what();
                        result.success = false;
                    }
                    catch (...){
                        result.error = "uknown error.";
                        result.success = false;
                    }
                    return result;
                }


            }//namespace detail




            chain_api::chain_api():my(new detail::chain_api_impl()) {
                JSON_RPC_REGISTER_API( NEWS_CHAIN_API_PLUGIN_NAME );
            }

            chain_api::~chain_api() {

            }

            DEFINE_LOCKLESS_APIS( chain_api,
                                  (push_block)
                                          (push_transaction)
            )


        }}}//news::plugins::chain