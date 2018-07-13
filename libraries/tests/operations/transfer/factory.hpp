#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>
namespace factory{

    using namespace news::chain;
    using namespace news::base;

    class helper{
    public:
        signed_transaction transfer(private_key_type sign_pk, account_name from, account_name to, asset num);
    };

    std::string  string_json_rpc(const std::string &str);
}