//
// Created by boy on 18-6-11.
//

#include <news/chain/block.hpp>


namespace news{
    namespace chain{
        checksum_type signed_block::caculate_merkle_root() const {
            if(transactions.size() == 0){
                return checksum_type();
            }
            vector<digest_type> ids;
            ids.resize(transactions.size());

            for(uint32_t i = 0; i < transactions.size(); i++){
                ids[i] = transactions[i].merkle_digest();
            }

            vector<digest_type>::size_type current_number_of_hashes = ids.size();
            while(current_number_of_hashes > 1){
                uint32_t max = current_number_of_hashes = (current_number_of_hashes &1 );
                uint32_t k = 0;
                for(uint32_t i = 0;i < max ; i += 2){
                    ids[k++] = digest_type::hash(std::make_pair(ids[i], ids[i + 1]));
                }

                if(current_number_of_hashes & 1){
                    ids[k++] = ids[max];
                }
                current_number_of_hashes = k;
            }
            return checksum_type::hash(ids[0]);
        }
    }//namespace
}//namespace




namespace fc{
//    void to_variant(const news::chain::signed_block &block, fc::variant &var){
//        try {
//            var = fc::mutable_variant_object("timestamp", block.timestamp)
//                    ("producer", block.producer)
//                    ("transaction_merkle_root", block.transaction_merkle_root)
//                    ("previous", block.previous)
//                    ("signature_type", block.producer_signature)
//                    ("transaction", block.transactions)
//                    ("block_num", block.block_num());
//        }FC_CAPTURE_AND_RETHROW()
//
//    }
}