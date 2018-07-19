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

        auto start = fc::time_point::now();

        auto ff = factory::helper();
        srand((unsigned) time(NULL));

        auto name = (account_name) (rand());
        auto str = ff.create_account(NEWS_INIT_PRIVATE_KEY, 1, name);

        std::string ret = string_json_rpc(fc::json::to_string(str));
        ddump((ret));


        auto end = fc::time_point::now();
        ilog("time:${t}", ("t", end - start));

    } catch (const fc::exception &e) {
        std::cout << e.to_detail_string() << std::endl;
    } catch (...) {
        std::cout << "unhandle exception." << std::endl;
    }


    return 0;
}


