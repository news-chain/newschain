//
// Created by boy on 18-7-16.
//

#pragma once

#include <string>

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/variant.hpp>

#include <boost/any.hpp>

namespace tools{





    struct result_body{
        std::string jsonrpc;
        fc::optional<fc::variant> result;
        fc::optional<fc::variant> error;
        int64_t     id;
    };


    bool result_has_error(const result_body &body);
}




FC_REFLECT(tools::result_body, (jsonrpc)(result)(error)(id))
