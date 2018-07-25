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

int main(int argc, char **argv) {

    try {
//        http::client client("ws://192.168.2.180:6002");
//
//        client.init();
//
//        client.set_handle_message([](const std::string &str){
//            tools::result_body ret = fc::json::from_string(str).as<tools::result_body>();
//            if(ret.error.valid()){
//                std::cout << str << std::endl;
//            }
//        });
//
        auto ff = factory::helper();
        srand((unsigned) time(NULL));
//

        auto posting = fc::ecc::private_key::generate();
        auto owner = fc::ecc::private_key::generate();

        auto name = (account_name) (rand());
        auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name, posting.get_public_key(), owner.get_public_key());
        std::string ret1 = string_json_rpc(fc::json::to_string(str));
        ddump((ret1));


        auto trx1 = ff.create_comment(posting, name, "123144423");
        std::string ret = string_json_rpc(fc::json::to_string(trx1));
        ddump((ret));

//        for(int j = 0; j < 100; j++){
//            for(int i = 0; i < 2000; i++){
//                auto start = fc::time_point::now();
//
//                auto name = (account_name) (rand());
//                auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);
//
//                std::string ret = string_json_rpc(fc::json::to_string(str));
////                ddump((ret));
//
//
//                auto end = fc::time_point::now();
////                ilog("time:${t}", ("t", end - start));
//
//                client.send_message(ret);
//
//
//            }
//            sleep(1);
//        }
//
//        client.stop();









    } catch (const fc::exception &e) {
        std::cout << e.to_detail_string() << std::endl;
    } catch (...) {
        std::cout << "unhandle exception." << std::endl;
    }


    return 0;
}


