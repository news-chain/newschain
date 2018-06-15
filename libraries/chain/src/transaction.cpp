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