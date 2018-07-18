#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>

namespace vdafactory
{
    using namespace news::chain;
    using namespace news::base;

    class helper {
    public:
        signed_transaction create_account(private_key_type sign_pk, account_name creator, account_name name);
        signed_transaction transfer(private_key_type sign_pk, account_name from, account_name to, asset num);
        signed_transaction transfers(private_key_type sign_pk, account_name from, std::map<account_name, asset> tos);
    };

    std::string  string_json_rpc(const std::string &str);
}