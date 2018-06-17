//
// Created by boy on 18-6-11.
//

#include <news/chain/block.hpp>


namespace news{
    namespace chain{
        checksum_type signed_block::caculate_merkle_root() const {
//            if(transactions.size() == 0){
//                return checksum_type();
//            }
            //TODO : transactions.size > 0
            return checksum_type();
        }
    }//namespace
}//namespace




namespace fc{
    void to_variant(const news::chain::signed_block &block, fc::variant &var){
        try {
            var = fc::mutable_variant_object("timestamp", block.timestamp)
                    ("producer", block.producer)
                    ("transaction_merkle_root", block.transaction_merkle_root)
                    ("previous", block.previous)
                    ("signature_type", block.producer_signature)
                    ("transaction", block.transactions)
                    ("block_num", block.block_num());
        }FC_CAPTURE_AND_RETHROW()

    }
}