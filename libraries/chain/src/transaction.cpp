//
// Created by boy on 18-6-12.
//


#include "news/chain/transaction.hpp"

namespace news{
    namespace chain{

        transaction_id_type transaction::id() {
            auto hh = digest();
            transaction_id_type result;
            memcpy(result._hash, hh._hash, std::min(sizeof(result), sizeof(hh)));
            return result;
        }

        digest_type transaction::digest() const {
            digest_type::encoder enc;
            fc::raw::pack(enc, *this);
            return enc.result();
        }


    }//namespace chain
}//namespace news



namespace  fc{
    void to_variant(news::chain::signed_transaction &strx, fc::variant &vo){
        try {
            vo = fc::mutable_variant_object("ref_block_num", strx.ref_block_num)
                    ("ref_block_prefix", strx.ref_block_prefix)
                    ("expiration", strx.expiration)
                    ("id", strx.id());
        }FC_CAPTURE_AND_RETHROW()
    }

    void from_variant(const fc::variant &var, news::chain::signed_transaction &trx){

    }

    void to_variant(news::chain::transaction &trx, fc::variant &vo){
        try {
            vo = fc::mutable_variant_object("ref_block_num", trx.ref_block_num)
                    ("ref_block_prefix", trx.ref_block_prefix)
                    ("expiration", trx.expiration);
        }FC_CAPTURE_AND_RETHROW()
    }

    void from_variant(const fc::variant &var, news::chain::transaction &trx){

    }
}