//
// Created by boy on 18-7-30.
//






#include <test/application.hpp>
#include <iostream>
#include <fc/exception/exception.hpp>


namespace test{


    namespace  detail{

        class application_impl{
        public:
            application_impl(){}
            ~application_impl(){}


//            bpo::options_description    _op_desc;
            bpo::options_description    _cfg_desc;

            bpo::variables_map          _map_args;

//            std::vector<http::client>   _clients;
        };


    }





    application::application() {

    }

    void application::set_program_args(int argc, char **argv) {

        my->_cfg_desc.add_options()
                ("help", "help message.")
                ("type", bpo::value<uint16_t>()->default_value(0), "1 create_account, 2 create_accounts, 3 transfer, 4 transfers")
                ("producer-threads", bpo::value<uint16_t>()->default_value(0), "create json threads.")
//                ("threads", bpo::value<uint16_t >()->default_value(1), "")
                ("websocket", bpo::value<std::vector<std::string>>(), "webserver address.")
                ("trx-ops", bpo::value<uint16_t >()->default_value(1), "how many operations in transactions");



        bpo::store(bpo::parse_command_line(argc, argv, my->_cfg_desc), my->_map_args);
//        bpo::store(bpo::parse_command_line(argc, argv, _op_desc), _map_args);



    }

    void application::start() {
        if( my->_map_args.count("help")){
            std::cout << my->_cfg_desc << std::endl;
            return;
        }
        if( my->_map_args.count("websocket")){
            std::vector<std::string> address =  my->_map_args["websocket"].as<std::vector<std::string>>();
            FC_ASSERT(address.size() > 0, "must once server address.");

            for(const auto &ad : address){
                http::client client(ad);
                client.init();
                client.set_handle_message([&](const std::string &str){
                    handle_message(str);
                });
            }
        }




    }

    void application::handle_message(const std::string &message) {

    }
}