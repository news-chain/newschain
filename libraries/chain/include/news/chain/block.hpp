//
// Created by boy on 18-6-11.
//

#pragma once


#include <news/chain/block_header.hpp>
#include <news/chain/transaction.hpp>


#include <fstream>


namespace news{
    namespace chain{


        struct signed_block : public signed_block_header{
            checksum_type                       caculate_merkle_root() const;
            std::vector<transaction>            transactions;
        };



    }//namespace chain
}//namespace news


FC_REFLECT_DERIVED(news::chain::signed_block, (news::chain::signed_block_header), (transactions))