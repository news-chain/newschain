//
// Created by boy on 18-6-26.
//



#include "factory.hpp"

namespace factory{

    signed_transaction helper::create_account(news::base::private_key_type sign_pk, news::base::account_name creator,
                                              news::base::account_name name) {
        signed_transaction trx;

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 30));

        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
        cao.public_key = fc::ecc::private_key::generate().get_public_key();

        trx.operations.push_back(cao);
        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }



    std::string  string_json_rpc(const std::string &str){
            std::string ret;
            ret = "{\"jsonrpc\":\"2.0\",\"params\":[\"chain_api\",\"push_transaction\"," + str +"],\"id\":-1,\"method\":\"call\"}";
            return ret;
    }
}