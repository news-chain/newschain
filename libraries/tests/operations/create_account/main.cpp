//
// Created by boy on 18-6-20.
//


#include <fc/exception/exception.hpp>
#include <iostream>

#include <wsClient.hpp>

#include <fc/io/json.hpp>
#include "factory.hpp"
#include "types.hpp"

using namespace factory;

int main(int argc, char **argv){

    try {



        for(int i = 0; i < 1; i++){
            std::thread([](){
//<<<<<<< HEAD
//                http::client client("ws://192.168.2.180:8001");
                http::client client("ws://192.168.56.128:8002");
                //http::client client("http://192.168.56.128:8001");
//=======
//                http::client client("ws://192.168.2.180:7002");
//>>>>>>> 5632fce04ce3899206a01bfbb8ea708089067598
                client.init();



//                client.set_handle_message([](std::string msg){
////                    std::cout << "handle msg : " << msg << std::endl;
//                    tools::result_body body = fc::json::from_string(msg).as<tools::result_body>();
//                    if(body.error.valid()){
//                       elog("error:${e}", ("e", body));
//                    }
//                });


                auto start = fc::time_point::now();

                auto ff =  factory::helper();
                srand((unsigned)time(NULL));
//<<<<<<< HEAD
                for(int j = 75; j < 76; j++){

//                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, (account_name)j/*(rand())*/);
                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, (account_name)(rand()));
//=======
//                for(int i = 1; i < 2; i++){
//
//                    auto name = (account_name)(rand());
//                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);
//
//>>>>>>> 5632fce04ce3899206a01bfbb8ea708089067598
                    std::string ret = string_json_rpc(fc::json::to_string(str));
                    ddump((ret));
                    //dlog(ret);
                    client.send_message(ret);


                }
                auto end = fc::time_point::now();
                ilog("time:${t}",("t", end - start));
                client.start();
            }).join();

        }

        //while(true);




    }catch (const fc::exception &e){
        std::cout << e.to_detail_string() << std::endl;
    }catch (...){
        std::cout << "unhandle exception." << std::endl;
    }






    return 0;
}


