//
// Created by boy on 18-5-30.
//


#include <app/application.hpp>
#include <news/plugins/webserver/webserver_plugin.hpp>
#include <news/plugins/producer_plugin/producer_plugin.hpp>
//#include <news/plugins/chain_plugin/chain_plugin.hpp>

#include <news/plugins/block_api_plugin/block_api_plugin.hpp>
#include <news/plugins/chain_api/chain_api_plugin.hpp>
#include <news/plugins/database_api/database_api_plugin.hpp>

int main(int argc, char *argv[]){

    std::cerr << "------------------------------------------" << std::endl;

    std::cerr << "NEWS CHAIN" << std::endl;
    std::cerr << "chain_id: " << std::string(NEWS_CHAIN_ID) << std::endl;
    std::cerr << "public_key: " << NEWS_INIT_PUBLIC_KEY<< std::endl;
//    std::cerr << "private_key: " << NEWS_INIT_PRIVATE_KEY << std::endl;
    std::cerr << "------------------------------------------" << std::endl;


    try {

        auto& app = news::app::application::getInstance();
//        app.register_plugin< news::plugins::chain_plugin::chain_plugin >();
        app.register_plugin< news::plugins::webserver::webserver_plugin >();
        app.register_plugin< news::plugins::block_api_plugin::block_api_plugin >();
        app.register_plugin< news::plugins::block_api_plugin::block_api_plugin >();
        app.register_plugin< news::plugins::chain_api_plugin::chain_api_plugin >();
        app.register_plugin< news::plugins::producer_plugin::producer_plugin >();
        app.register_plugin< news::plugins::database_api::database_api_plugin >();


        bool init = app.initizlize<news::plugins::producer_plugin::producer_plugin,
                news::plugins::block_api_plugin::block_api_plugin,
                news::plugins::chain_api_plugin::chain_api_plugin,
                news::plugins::database_api::database_api_plugin,
                news::plugins::webserver::webserver_plugin>(argc, argv);
        if(!init){
            std::cout << "application init error " << std::endl;
            return -1;
        }



        app.start_up();
        app.exec();
    }catch (...){
        return -1;
    }
    return 0;
}