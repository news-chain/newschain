//
// Created by boy on 18-6-12.
//


#include <fc/bitutil.hpp>
#include "news/chain/transaction.hpp"


namespace news{
    namespace chain{

        transaction_id_type transaction::id() const{
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

        void transaction::validate() const {
            FC_ASSERT( operations.size() > 0, "A transaction must have at least one operation", ("trx",*this) );
            for(const auto &op : operations){
                op.visit(operation_validate_visitor());
            }
        }

        digest_type transaction::sig_digest(const chain_id_type &chain_id) const {
            digest_type::encoder enc;
            fc::raw::pack(enc, *this);
            return enc.result();
        }

        void transaction::set_expiration(fc::time_point_sec expiration_time) {
            expiration = expiration_time;
        }

        void transaction::set_reference_block(const block_id_type &reference_block) {
            ref_block_num = fc::endian_reverse_u32(reference_block._hash[0]);
            ref_block_prefix = reference_block._hash[1];
        }


        const signature_type &signed_transaction::sign(const private_key_type &pk, const chain_id_type &chain_id) {
            digest_type h = sig_digest(chain_id);
            signatures.push_back(pk.sign_compact(h));
            return signatures.back();
        }

        signature_type signed_transaction::sign(const private_key_type &pk, const chain_id_type &chain_id) const {
            digest_type::encoder ec;
            fc::raw::pack(ec, chain_id);
            fc::raw::pack(ec, *this);

            return pk.sign_compact(ec.result());
        }

        digest_type signed_transaction::merkle_digest() const {
            digest_type::encoder ec;
            fc::raw::pack(ec, *this);
            return ec.result();
        }

        flat_set<public_key_type> signed_transaction::get_signature_keys(const chain_id_type &chain_id) const {
            try {
                auto d = sig_digest(chain_id);
                flat_set<public_key_type> result;
                for(const auto &sig : signatures){
                    //TODO throw exception  .second == true
                    result.insert(fc::ecc::public_key(sig, d));
                }
                return result;
            }FC_CAPTURE_AND_RETHROW()
        }

        void signed_transaction::verify_authority(const get_key_by_name &get_key, const chain_id_type &chain_id) const{
            flat_set<public_key_type> pubs = this->get_signature_keys(chain_id);
            std::map<public_key_type, bool> used;
            flat_set<account_name > names;
            for(auto &p : pubs){
                used[p] = false;
            }

            for(auto &op : operations){
                op.visit(operation_get_sign_name_visitor(names));
            }

            for(auto &name :names){
                auto pk = get_key(name);
                FC_ASSERT(used.find(pk) != used.end(), "signed error : user name ${n}, public_key:${p}", ("n", name)("p", pk.key_data));
                used[pk] = true;
            }

            for(auto unused_key : used){
               FC_ASSERT(unused_key.second, "check unnecessary sign.${p}", ("p", unused_key.first));
            }

        }


    }//namespace chain
}//namespace news



namespace  fc{
//    void to_variant(const news::chain::signed_transaction &strx, fc::variant &vo){
//        try {
//            vo = fc::mutable_variant_object("ref_block_num", strx.ref_block_num)
//                    ("ref_block_prefix", strx.ref_block_prefix)
//                    ("expiration", strx.expiration);
//        }FC_CAPTURE_AND_RETHROW()
//    }
//
//    void from_variant(const fc::variant &var, news::chain::signed_transaction &trx){
//
//    }

//    void to_variant(const news::chain::transaction &trx, fc::variant &vo){
//        try {
//            vo = fc::mutable_variant_object("ref_block_num", trx.ref_block_num)
//                    ("ref_block_prefix", trx.ref_block_prefix)
//                    ("expiration", trx.expiration);
//        }FC_CAPTURE_AND_RETHROW()
//    }
//
//    void from_variant(const fc::variant &var, news::chain::transaction &trx){
//
//    }
}