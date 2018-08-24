//
// Created by boy on 18-6-16.
//


#include <fc/reflect/reflect.hpp>
#include <fc/log/logger.hpp>
#include <fc/io/varint.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/io/json.hpp>

#include <news/chain/global_property_object.hpp>

int main(int argc, char **argv) {

    news::chain::dynamic_global_property_object object;
    fc::variant_object oo = fc::variant(object).as<fc::variant_object>();
    for(auto itr = oo.begin(); itr != oo.end(); itr++){
        elog("key = : ${t}", ("t", itr->key()));
        elog("key = : ${t}", ("t", itr->value()));
    }



    return 0;
}