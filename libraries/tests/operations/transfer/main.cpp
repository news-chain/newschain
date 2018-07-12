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
//                http::client client("ws://192.168.2.180:8001");
                http::client client("ws://192.168.56.128:8001");
                client.init();

                auto start = fc::time_point::now();

                auto ff =  factory::helper();
                srand((unsigned)time(NULL));
                for(int j = 1; j < 2; j++){

                    auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, (account_name)(rand()));
                    std::string ret = string_json_rpc(fc::json::to_string(str));
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