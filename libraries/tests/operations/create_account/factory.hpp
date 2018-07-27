//
// Created by boy on 18-6-26.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>

#include <news/chain/global_property_object.hpp>
#include <news/chain/block_header.hpp>
namespace factory{


    using namespace news::chain;
    using namespace news::base;

    class helper{
    public:
        signed_transaction create_account(private_key_type sign_pk, account_name creator, account_name name);
        signed_transaction create_transfer(private_key_type sign_pk, account_name from, account_name to, asset amount);


        std::string     get_string_dynamic_property();


        dynamic_global_property_object  property_object;
    };



    std::string  string_json_rpc(const std::string &str);

}