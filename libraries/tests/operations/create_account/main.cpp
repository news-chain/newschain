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


