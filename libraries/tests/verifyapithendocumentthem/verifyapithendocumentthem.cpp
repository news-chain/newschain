//
// Created by oy on 18-7-2018.
//

#include <fc/exception/exception.hpp>
#include <iostream>

#include <wsClient.hpp>

#include <fc/io/json.hpp>
#include "vdafactory.hpp"
#include <news/plugins/database_api/database_api.hpp>

using namespace vdafactory;

int main(int argc, char **argv)
{
    try
    {
        news::plugins::database_api::get_transactions_hex_args args;
        //args.trx.
        ddump((args));

        //ut for transfer
        for(int i = 0; i < 1; i++)
        {
            std::thread([](){
                //http::client client("ws://192.168.2.180:8001");
                http::client client("ws://192.168.56.128:8002");
                client.init();

                auto start = fc::time_point::now();

                auto ff =  vdafactory::helper();
                srand((unsigned)time(NULL));
                for(int j = 1; j < 2; j++)
                {
                    auto str = ff.transfer(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, NEWS_SYSTEM_ACCOUNT_NAME + 1, asset(NEWS_SYMBOL, 1));
                    std::string ret = string_json_rpc(fc::json::to_string(str));
                    dlog(ret);
                    client.send_message(ret);
                }
                auto end = fc::time_point::now();
                ilog("time:${t}",("t", end - start));
                client.start();
                //}).detach();
            }).join();
        }

        //while(true);

        //ut for transfers
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