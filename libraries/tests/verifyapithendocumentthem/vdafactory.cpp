#include "vdafactory.hpp"

namespace vdafactory
{
    signed_transaction helper::create_account(news::base::private_key_type sign_pk, news::base::account_name creator,
                                              news::base::account_name name)
    {
        signed_transaction trx;

        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
        cao.public_key = fc::ecc::private_key::generate().get_public_key();


        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 1000));
        trx.operations.push_back(cao);
        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }

    signed_transaction helper::transfer(news::base::private_key_type sign_pk, news::base::account_name from,
                                              news::base::account_name to, news::base::asset num)
    {
        signed_transaction trx;

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));

        transfer_operation op;
        op.from = from;
        op.to = to;
        op.amount = num;

        trx.operations.push_back(op);
        trx.sign(sign_pk, NEWS_CHAIN_ID);

        return trx;
    }

    signed_transaction helper::transfers(news::base::private_key_type sign_pk, news::base::account_name from,
                                         std::map<account_name, asset> tos)
    {
        signed_transaction trx;

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));

        transfers_operation op;
        op.from = from;
        op.to_names = tos;

        trx.operations.push_back(op);
        trx.sign(sign_pk, NEWS_CHAIN_ID);

        return trx;
    }

    std::string assemble_json_rpc_string__get_account_history(const std::string &str)
    {
        std::string ret;
        ret = R"rs({"jsonrpc":"2.0","params":["account_history_api","get_account_history",{"name":1,"start":4294967295,"limit":1000}],"id":-1,"method":"call"})rs";
        return ret;
    }

    std::string assemble_json_rpc_string__get_transaction(const std::string &str)
    {
        std::string ret;
        ret = R"rs({"jsonrpc":"2.0","params":["account_history_api","get_transaction",{"trx_id":"5eb267aef6fa66fa5be0feaaa4ba318e394bfa32"}],"id":-1,"method":"call"})rs";
        return ret;
    }

    std::string  string_json_rpc(const std::string &str)
    {
            std::string ret;
            ret = R"rs({"jsonrpc":"2.0","params":["network_broadcast_api","broadcast_transaction",{"trx":)rs" + str + R"rs(}],"id":-1,"method":"call"})rs";
            return ret;
    }
}