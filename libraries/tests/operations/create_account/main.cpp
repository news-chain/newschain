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
        http::client client("ws://192.168.0.139:6002");
//        http::client client1("ws://192.168.0.139:6002");

        client.init();
//        client1.init();
        auto ff = factory::helper();


        client.set_handle_message([&](const std::string &str) {
            tools::result_body ret = fc::json::from_string(str).as<tools::result_body>();
            if (!ret.error.valid()) {
                std::cout << str << std::endl;
            }
            if(ret.id == 200000){
                std::cout << str << std::endl;
                auto pp = ret.result->as<dynamic_global_property_object>();
                ff.property_object = pp;
            }
        });

//        client1.set_handle_message([](const std::string &str) {
//            tools::result_body ret = fc::json::from_string(str).as<tools::result_body>();
//            if (ret.error.valid()) {
//                std::cout << str << std::endl;
//            }
//        });




        sleep(2);


        auto api = ff.get_string_dynamic_property();
        client.send_message(api);


        srand((unsigned) time(NULL));

        uint64_t name = 5000000;
//        while (true) {
//            for (int i = 0; i < 500; i++) {
                auto start = fc::time_point::now();


                auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);

                std::string ret = string_json_rpc(fc::json::to_string(str));
                ddump((ret));


                auto end = fc::time_point::now();
//                ilog("time:${t}", ("t", end - start));
//                if (i % 2 == 0) {
//                    client.send_message(ret);
//                } else {
//                    client1.send_message(ret);
//                }
                name++;


//            }
//            sleep(1);
//        }

        client.stop();
//        client1.stop();


    } catch (const fc::exception &e) {
        std::cout << e.to_detail_string() << std::endl;
    } catch (...) {
        std::cout << "unhandle exception." << std::endl;
    }


    return 0;
}


