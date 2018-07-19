//
// Created by oy on 18-7-2018.
//

#include <iostream>

#include <fc/exception/exception.hpp>
#include <fc/io/json.hpp>
#include "fc/crypto/ripemd160.hpp"
#include <news/plugins/database_api/database_api.hpp>
#include "news/plugins/account_history_api/account_history_api.hpp"
#include <wsClient.hpp>

#include "vdafactory.hpp"

using namespace vdafactory;

int main(int argc, char **argv)
{
    try
    {

        //verify account history
        news::plugins::account_history_api::get_transaction_args _getx_args;
        _getx_args.trx_id = fc::ripemd160("5eb267aef6fa66fa5be0feaaa4ba318e394bfa32");
        ddump((_getx_args));
        std::string strGetTx = assemble_json_rpc_string__get_transaction("test");
        ddump((strGetTx));

        news::plugins::account_history_api::get_account_history_args _args;
        _args.name = NEWS_SYSTEM_ACCOUNT_NAME;
        _args.start = -1;
        _args.limit = 1000;
        ddump((_args));
        //std::string strAccHis = string_json_rpc("test");
        std::string strAccHis = assemble_json_rpc_string__get_account_history("test");
        ddump((strAccHis));

        news::plugins::account_history_api::get_account_history_return _ret;
        ddump((_ret));






        //verify hashing transaction
        news::plugins::database_api::get_transactions_hex_args args;
        args.trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 5));   //set timer as 5 seconds
        args.trx.sign(NEWS_INIT_PRIVATE_KEY, NEWS_CHAIN_ID);
        ddump((args));

        //verify creating account
//        std::thread([](){
//            http::client client("ws://192.168.56.128:8002");
//            //http::client client("http://192.168.56.128:8001");
//            client.init();
//
//            auto start = fc::time_point::now();
//
//            auto ff =  vdafactory::helper();
//            srand((unsigned)time(NULL));
//            for(int j = 72; j < 75; j++){
//
//                auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, (account_name)j/*(rand())*/);
//                std::string ret = string_json_rpc(fc::json::to_string(str));
//                ddump((ret));
//                //dlog(ret);
//  //              client.send_message(ret);
//
//            }
//            auto end = fc::time_point::now();
//            ilog("time:${t}",("t", end - start));
//            client.start();
//        }).join();




        //verify transferring asset(1 to 1)
//        for(int i = 0; i < 1; i++)
//        {
//            std::thread([](){
//                //http::client client("ws://192.168.2.180:8001");
//                http::client client("ws://192.168.56.128:8002");
//                client.init();
//
//                auto start = fc::time_point::now();
//
//                auto ff =  vdafactory::helper();
//                srand((unsigned)time(NULL));
//                for(int j = 1; j < 2; j++)
//                {
//                    auto str = ff.transfer(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, NEWS_SYSTEM_ACCOUNT_NAME + 136, asset(NEWS_SYMBOL, 1));
//                    std::string ret = string_json_rpc(fc::json::to_string(str));
//                    dlog(ret);
//                    client.send_message(ret);
//                }
//                auto end = fc::time_point::now();
//                ilog("time:${t}",("t", end - start));
//                client.start();
//                //}).detach();
//            }).join();
//        }
        //while(true);

        //verify transferring asset(1 to n)
        for(int i = 0; i < 1; i++)
        {
            std::thread([](){
                //http::client client("ws://192.168.2.180:8001");
                http::client client("ws://192.168.56.128:8002");
                client.init();

                auto start = fc::time_point::now();

                auto ff =  vdafactory::helper();
                srand((unsigned)time(NULL));
                for(int j = 0; j < 1; j++)
                {
                    std::map<account_name, asset> ts;
                    ts[71] = asset(NEWS_SYMBOL, 10);
                    ts[72] = asset(NEWS_SYMBOL, 10);
                    ts[73] = asset(NEWS_SYMBOL, 10);
                    ts[74] = asset(NEWS_SYMBOL, 10);
                    auto str = ff.transfers(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, ts);
                    std::string ret = string_json_rpc(fc::json::to_string(str));
                    dlog(ret);
                    client.send_message(ret);
                }
                auto end = fc::time_point::now();
                ilog("time:${t}",("t", end - start));
                client.start();
            }).join();
        }

    }
    catch (const fc::exception &e)
    {
        std::cout << e.to_detail_string() << std::endl;
    }

    return 0;
}