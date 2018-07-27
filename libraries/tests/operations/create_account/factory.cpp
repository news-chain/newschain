//
// Created by boy on 18-6-26.
//



#include "factory.hpp"

namespace factory{

    signed_transaction helper::create_account(news::base::private_key_type sign_pk, news::base::account_name creator,
                                              news::base::account_name name) {
        signed_transaction trx;


        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
        cao.posting = {fc::ecc::private_key::generate().get_public_key(),1};
        cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t)(property_object.head_block_num & 0xffff);

        trx.operations.push_back(cao);
        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }

    signed_transaction
    helper::create_transfer(private_key_type sign_pk, account_name from, account_name to, asset amount) {
            signed_transaction trx;

            transfer_operation transfer;
            transfer.from = from;
            transfer.to = to;
            transfer.amount = amount;
            trx.operations.push_back(transfer);

            trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
            trx.sign(sign_pk, NEWS_CHAIN_ID);

            return trx;
    }

    std::string helper::get_string_dynamic_property() {
        return "{\"jsonrpc\":\"2.0\",\"params\":[\"database_api\",\"get_dynamic_global_property\", {}],\"id\":200000,\"method\":\"call\"} ";
    }


    std::string  string_json_rpc(const std::string &str){
            std::string ret;
            ret = "{\"jsonrpc\":\"2.0\",\"params\":[\"network_broadcast_api\",\"broadcast_transaction\",{\"trx\":" + str +"}],\"id\":-1,\"method\":\"call\"}";
            return ret;
    }
}