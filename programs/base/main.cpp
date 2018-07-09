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
#include <app/logs.hpp>
#include <news/base/key_conversion.hpp>
#include "news/plugins/p2p/p2p_plugin.hpp"


void regist_plugin(news::app::application &app){
    app.register_plugin< news::plugins::chain_plugin::chain_plugin >();
    app.register_plugin< news::plugins::webserver::webserver_plugin >();
    app.register_plugin< news::plugins::block_api_plugin::block_api_plugin >();
    app.register_plugin< news::plugins::chain_api_plugin::chain_api_plugin >();
    app.register_plugin< news::plugins::producer_plugin::producer_plugin >();
    app.register_plugin< news::plugins::database_api::database_api_plugin >();
    app.register_plugin< news::plugins::p2p::p2p_plugin >();
}




int main(int argc, char **argv){

    std::cerr << "------------------------------------------" << std::endl;

    std::cerr << "NEWS CHAIN" << std::endl;
    std::cerr << "chain_id: " << std::string(NEWS_CHAIN_ID) << std::endl;
    std::cerr << "public_key: " << NEWS_INIT_PUBLIC_KEY<< std::endl;
    std::cerr << "private_key: " << (news::base::key_to_wif(NEWS_INIT_PRIVATE_KEY.get_secret())) << std::endl;
    std::cerr << "------------------------------------------" << std::endl;



    try {
        boost::program_options::options_description options;
        boost::program_options::options_description desc;

        news::app::set_logging_program_options(options);


        auto &app = news::app::application::getInstance();
        app.add_program_options(desc, options);

        regist_plugin(app);

        app.set_default_plugins<
                news::plugins::p2p::p2p_plugin,
                news::plugins::chain_plugin::chain_plugin
        >();

        bool init = app.initizlize<
                news::plugins::producer_plugin::producer_plugin,
                news::plugins::block_api_plugin::block_api_plugin,
                news::plugins::chain_api_plugin::chain_api_plugin,
                news::plugins::database_api::database_api_plugin,
                news::plugins::p2p::p2p_plugin,
                news::plugins::webserver::webserver_plugin
        >(argc, argv);

        if(!init){
            std::cout << "application init error " << std::endl;
            return -1;
        }

        auto config = news::app::load_logging_config(app.get_args(), app.get_data_path());
        if(config){
            fc::configure_logging(*config);
        }
        else{
            std::cerr << "load config error " << std::endl;
        }

        app.start_up();

        app.exec();
    }catch (const std::exception &e){
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}