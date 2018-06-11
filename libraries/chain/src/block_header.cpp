//
// Created by boy on 18-6-11.
//


#include <news/chain/block_header.hpp>



namespace news{
    namespace chain{


        digest_type block_header::digest() const {
            return digest_type::hash(*this);
        }




    }//namespace chain
}//namesapce news