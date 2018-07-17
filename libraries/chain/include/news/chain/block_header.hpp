//
// Created by boy on 18-6-11.
//

#pragma once


#include <fc/time.hpp>
#include <news/base/types.hpp>
#include <fc/crypto/sha224.hpp>
#include <fc/bitutil.hpp>
#include <news/base/config.hpp>

namespace news{
    namespace chain{

        using namespace news::base;



        struct block_header{
            account_name                    producer;
            checksum_type                   transaction_merkle_root;
            block_id_type                   previous;
            news::base::version             version;
            fc::time_point_sec              timestamp;
            digest_type                     digest() const;

            uint32_t                        block_num() const {return fc::endian_reverse_u32(previous._hash[0]) + 1;}

            static uint32_t                 num_from_id(const block_id_type &id);

        };


        struct signed_block_header : public block_header{
            block_id_type                   id() const;
            fc::ecc::public_key             signee() const;
            void                            sign(const fc::ecc::private_key &signer);
            bool                            validate_signee(const fc::ecc::public_key &expected_signee) const;
            signature_type                  producer_signature;
        };


    }//namespace chain
}//namespace news

FC_REFLECT(news::chain::block_header, (producer)(transaction_merkle_root)(previous)(version)(timestamp))
FC_REFLECT_DERIVED(news::chain::signed_block_header, (news::chain::block_header), (producer_signature))