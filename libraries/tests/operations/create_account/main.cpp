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
<<<<<<< HEAD
//                http::client client("ws://192.168.2.180:8001");
                http::client client("ws://192.168.56.128:8001");
=======
                http::client client("ws://192.168.2.180:6002");
>>>>>>> 9bcf81ea63a851c70881930262477efcb393276c
                client.init();

                auto start = fc::time_point::now();

                auto ff =  factory::helper();
                srand((unsigned)time(NULL));
<<<<<<< HEAD
                for(int j = 1; j < 2; j++){
=======
                for(int i = 1; i < 100000; i++){
>>>>>>> 9bcf81ea63a851c70881930262477efcb393276c

                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, (account_name)(rand()));
                    std::string ret = string_json_rpc(fc::json::to_string(str));
//                    ddump((ret));
                    client.send_message(ret);
                }
                auto end = fc::time_point::now();
                ilog("time:${t}",("t", end - start));
                client.start();
            }).detach();

        }

        while(true);



    }catch (const fc::exception &e){
        std::cout << e.to_detail_string() << std::endl;
    }




    return 0;
}