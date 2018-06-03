//
// Created by boy on 18-5-30.
//


#include <app/application.hpp>
#include <news/plugins/webserver/webserver_plugin.hpp>

int main(int argc, char *argv[]){


    try {
        auto& app = news::app::application::getInstance();
        app.register_plugin< news::plugins::webserver::webserver_plugin >();
        app.initizlize<news::plugins::webserver::webserver_plugin>(argc, argv);
    }catch (...){
        return -1;
    }
    return 0;
}