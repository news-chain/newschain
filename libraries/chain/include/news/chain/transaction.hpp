//
// Created by boy on 18-6-12.
//

#pragma once


//#include <cstdint>
#include <fc/time.hpp>
#include <fc/reflect/reflect.hpp>
#include <news/protocol/types.hpp>


namespace news{
    namespace chain{

        using namespace protocol;

        struct transaction{
            uint16_t                ref_block_num = 0;
            uint32_t                ref_block_prefix = 0;
            fc::time_point_sec      expiration;

            transaction_id_type     id();
            digest_type             digest() const;
        };



        struct signed_transaction : public transaction{

            std::vector<signature_type>     signatures;
        };


    }//namespace chain
}//namespace news




FC_REFLECT(news::chain::transaction, (ref_block_num)(ref_block_prefix)(expiration))
FC_REFLECT_DERIVED(news::chain::signed_transaction, (news::chain::transaction), (signatures))