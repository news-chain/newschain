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
            //TODO : transactions.size > 0
            return checksum_type();
        }
    }//namespace
}//namespace