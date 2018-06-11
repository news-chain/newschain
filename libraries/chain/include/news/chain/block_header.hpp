//
// Created by boy on 18-6-11.
//

#pragma once


#include <fc/time.hpp>
#include <news/protocol/types.hpp>


namespace news{
    namespace chain{

        using namespace news::protocol;



        struct block_header{
            fc::time_point_sec              timestamp;
            account_name                    producer;
            checksum_type                   transaction_merkle_root;
            block_id_type                   previous;
            digest_type                     digest() const;
            extendsions_type                extensions;
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




FC_REFLECT(news::chain::block_header, (timestamp)(producer)(transaction_merkle_root)(previous)(extensions))
FC_REFLECT_DERIVED(news::chain::signed_block_header, (news::chain::block_header), (producer_signature))