//
// Created by boy on 18-6-20.
//


#include <fc/exception/exception.hpp>
#include <iostream>

#include <wsClient.hpp>

#include <fc/io/json.hpp>
#include "factory.hpp"

using namespace factory;

int main(int argc, char **argv){

    try {



        for(int i = 0; i < 1; i++){
            std::thread([](){
                http::client client("ws://192.168.2.180:7002");
                client.init();

                auto start = fc::time_point::now();

                auto ff =  factory::helper();
                srand((unsigned)time(NULL));
                for(int i = 1; i < 2; i++){

                    auto name = (account_name)(rand());
                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);

                    std::string ret = string_json_rpc(fc::json::to_string(str));
                    ddump((ret));
                    client.send_message(ret);

                    auto tt = ff.create_transfer(NEWS_INIT_PRIVATE_KEY, 1, 2, asset(NEWS_SYMBOL, 2));
                    std::string ret1 =  string_json_rpc(fc::json::to_string(tt));
                    ddump((ret1));
                    client.send_message(ret1);

                }
                auto end = fc::time_point::now();
                ilog("time:${t}",("t", end - start));
                client.start();
            }).join();

        }

//        while(true);




    }catch (const fc::exception &e){
        std::cout << e.to_detail_string() << std::endl;
    }




    return 0;
}


