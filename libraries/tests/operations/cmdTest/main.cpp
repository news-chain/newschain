//
// Created by boy on 18-7-11.
//


#include <boost/program_options/options_description.hpp>


namespace  bpo = boost::program_options;
int main(int argc, char **argv){


    bpo::options_description op("command line");
    op.add_options()
            ("help,h", "help message")
            ("server,s", bpo::value<std::vector<std::string>>(), "websocket ip , port")
            ("thread,t", bpo::value<uint8_t>(), "one client start thread")
            ("client,c", bpo::value<uint8_t>(), "start clients");


    return  0;
}