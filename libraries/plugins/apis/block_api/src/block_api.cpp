//
// Created by boy on 18-6-20.
//

#include <news/plugins/block_api_plugin/block_api.hpp>
#include <news/plugins/json_rpc/json_rpc_plugin.hpp>
#include <news/plugins/block_api_plugin/block_api_plugin.hpp>


namespace news{
    namespace plugins{
        namespace block_api_plugin{

            class block_api_impl
            {
            public:
                block_api_impl();
                ~block_api_impl();

                DECLARE_API_IMPL(
                        (get_block_header)
                                (get_block)
                                (fetch_chunk)
                )

                news::chain::database& _db;
            };


            block_api::block_api():my(new block_api_impl()) {
                JSON_RPC_REGISTER_API( NEWS_BLOCK_API_PLUGIN_NAME );
            }

            block_api::~block_api() {

            }

            //////////////////////////////////////////////////////////////
                                    //impl
            //////////////////////////////////////////////////////////////

            block_api_impl::block_api_impl():_db(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>().get_database()) {}
            block_api_impl::~block_api_impl() {}



            DEFINE_API_IMPL(block_api_impl, get_block_header)
            {
                get_block_header_return result;
                auto block = _db.fetch_block_by_number(args.block_num);
                if(block){
                    result.header = *block;
                }
                return result;
            }

            DEFINE_API_IMPL(block_api_impl, get_block)
            {
                get_block_return result;
                auto block = _db.fetch_block_by_number(args.block_num);
                if(block){
                    result.block = *block;
                }
                return result;
            }

            //practice for interface implementation - by Oijen
            DEFINE_API_IMPL(block_api_impl, fetch_chunk)
            {
                fetch_chunk_return result;
                auto chunk = _db.fetch_block_by_number(args.chunk_num);
                if(chunk){
                    result.chunk = *chunk;
                }
                return result;
            }


            DEFINE_READ_APIS( block_api,
                              (get_block_header)
                                      (get_block)
                                      (fetch_chunk)
            )


        }}}//news::pllugins::block_api_plugin