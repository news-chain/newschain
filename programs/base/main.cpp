//
// Created by boy on 18-5-30.
//


#include <app/application.hpp>


int main(int argc, char *argv[]){


    try {
        auto& app = news::app::application::getInstance();
        app.initialize_impl(argc, argv);
    }catch (...){
        return -1;
    }
    return 0;
}