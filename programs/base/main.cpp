//
// Created by boy on 18-5-30.
//


#include <app/application.hpp>
#include <news/plugins/webserver/webserver_plugin.hpp>
#include <news/plugins/producer_plugin/producer_plugin.hpp>

int main(int argc, char *argv[]){


    try {

        auto& app = news::app::application::getInstance();
        app.register_plugin< news::plugins::producer_plugin::producer_plugin >();
        app.register_plugin< news::plugins::webserver::webserver_plugin >();
        bool init = app.initizlize<news::plugins::webserver::webserver_plugin>(argc, argv);
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