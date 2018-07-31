//
// Created by boy on 18-7-16.
//

#pragma once

#include <string>

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/variant.hpp>
#include <fc/time.hpp>
#include <boost/any.hpp>


namespace tools{





    struct result_body{
        std::string jsonrpc;
        fc::optional<fc::variant> result;
        fc::optional<fc::variant> error;
        int64_t     id;
    };



    struct send_body{
        std::string         jsonrpc;
        fc::variant         params;
        uint64_t            id;
        std::string         call;
    };


    struct get_context
    {
//        get_context(){}
//        ~get_context(){}

        uint64_t            id;
        fc::time_point      send_time;
        fc::time_point      get_time;
        send_body           send;
        result_body         ret;
    };



    bool result_has_error(const result_body &body);
}




FC_REFLECT(tools::result_body, (jsonrpc)(result)(error)(id))
FC_REFLECT(tools::send_body, (jsonrpc)(params)(id)(call))
