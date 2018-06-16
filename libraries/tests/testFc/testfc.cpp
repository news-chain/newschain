//
// Created by boy on 18-6-16.
//



#include <fc/reflect/reflect.hpp>
#include <fc/log/logger.hpp>
#include <fc/exception/exception.hpp>
struct transaction;




struct transaction{
    int trx;
    int id;
};



FC_REFLECT(transaction, (trx)(id))
//FC_REFLECT_TYPENAME(transaction)


namespace fc{
    void to_variant(const transaction& trx, fc::variant& var){
        try {
            var = fc::mutable_variant_object("trx", trx.trx)("id", trx.id);
        }FC_CAPTURE_AND_RETHROW()

    }

    void from_variant(const fc::variant &var, transaction &vo){
        try {
            FC_ASSERT(var.is_object(), "transaction has to object");
            const auto &obj = var.get_object();
            FC_ASSERT(obj.contains("trx"), "trx field doesnt exits");
            vo.trx = boost::lexical_cast<int>( obj["trx"].as<int>());

            FC_ASSERT(obj.contains("id"), "transaction doesnt has id");
            vo.id = boost::lexical_cast<int>( obj["id"].as<int>());

        }FC_CAPTURE_AND_RETHROW()

    }
}


int main(){
    transaction tt = {1, 1};
    elog("tt ${t}", ("t", tt));



    return 0;
}