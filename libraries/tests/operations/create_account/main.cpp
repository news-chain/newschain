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
        http::client client("ws://192.168.0.22:7000");
        http::client client1("ws://192.168.0.21:7000");

        client.init();
        client1.init();
        auto ff = factory::helper();






        auto func = [&](const std::string msg){
            tools::result_body ret = fc::json::from_string(msg).as<tools::result_body>();
            if (ret.error.valid()) {
                std::cout << "func : " <<  msg << std::endl;
            }
            if(ret.id == 200000){
                std::cout << "dynamic_global_property_object " <<  msg << std::endl;
                auto pp = ret.result->as<dynamic_global_property_object>();
                ff.property_object = pp;
            }
        };


        client.set_handle_message([&](const std::string &str) {
            func(str);
        });

        client1.set_handle_message([&](const std::string &str) {
            func(str);
        });


        auto api = ff.get_string_dynamic_property();
        client.send_message(api);


        srand((unsigned) time(NULL));

        uint64_t name = 20;


        fc::time_point update_time = fc::time_point::now();


        while (true) {
            for (int i = 0; i < 1000; i++) {
//                auto start = fc::time_point::now();


                auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);

                std::string ret = string_json_rpc(fc::json::to_string(str));
//                ddump((ret));


//                auto end = fc::time_point::now();
//                ilog("time:${t}", ("t", end - start));
                if (i % 2 == 0) {
                    client.send_message(ret);
                } else {
                    client1.send_message(ret);
                }
                name++;


            }
//            ilog("send get dynamic property.");
            if(fc::time_point::now() - update_time > fc::hours(1)){
                update_time = fc::time_point::now();
                auto api = ff.get_string_dynamic_property();
                client.send_message(api);
                sleep(1);
            }
        }
        client.stop();
        client1.stop();


    } catch (const fc::exception &e) {
        std::cout << e.to_detail_string() << std::endl;
    } catch (...) {
        std::cout << "unhandle exception." << std::endl;
    }


    return 0;
}


