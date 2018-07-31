//
// Created by boy on 18-7-30.
//

#pragma once


#include <boost/program_options.hpp>

#include <wsClient.hpp>
#include <memory>
//#include <unique_ptr.h>
namespace test{

    namespace bpo = boost::program_options;
//    typedef std::funcation<void(const std::string &msg>     server_callback;


    namespace detail{
        class application_impl;
    }



    class application{
    public:
        application();
        ~application();
        void set_program_args(int argc, char **argv);

        void start();



    private:
        void handle_message(const std::string &message);

    private:
        std::unique_ptr< detail::application_impl > my;
    };
}