//
// Created by boy on 18-6-20.
//


//#include <fc/exception/exception.hpp>
//#include <iostream>
//
//#include <wsClient.hpp>
//
//#include <fc/io/json.hpp>
//#include "factory.hpp"
//#include "types.hpp"

//using namespace factory;



#include <test/application.hpp>
#include <fc/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <fc/log/logger_config.hpp>
#include <test/types.hpp>

#include <fc/io/json.hpp>

int main(int argc, char **argv) {
//    std::string str = "{\"jsonrpc\":\"2.0\",\"result\":{\"id\":0,\"head_block_num\":2956,\"head_block_id\":\"00000b8c28883ea6e9ddadb8b4e021c94412a7d6\",\"time\":\"2018-08-13T03:31:06\",\"current_producer\":0,\"last_irreversible_block_num\":2935,\"current_aslot\":165822},\"error\":null,\"id\":1}";
//
//    tools::result_body ret;
//    fc::variant vv = fc::json::from_string(str);
//    ret = vv.as<tools::result_body>();
//    ilog("${s}", ("s", ret));

    try {
        auto config = fc::logging_config::default_config();
        fc::configure_logging(config);
        test::application app;
        app.set_program_args(argc, argv);
        app.start();

    }catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        return -1;
    }catch (const fc::assert_exception &e){
        std::cerr << e.to_detail_string() << std::endl;
        return 2;
    }catch (const fc::exception &e){
        std::cerr << "main exception : " << e.to_detail_string() << std::endl;
        return 2;
    } catch (const boost::exception &e){
        std::cerr << boost::diagnostic_information(e) << std::endl;
        return 3;
    }catch (...){
        std::cerr << "unhandle exception in main\n";// << __FUNCTION__ << __LINE__ << std::endl;
        return 4;
    }


    return 0;
}


