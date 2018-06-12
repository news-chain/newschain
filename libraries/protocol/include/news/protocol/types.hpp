//
// Created by boy on 18-6-11.
//

#pragma once


#include <fc/crypto/sha256.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/crypto/elliptic.hpp>

namespace news{
    namespace protocol{

        typedef fc::sha256                  digest_type;
        typedef fc::sha256                  checksum_type;
        typedef fc::ripemd160               transaction_id_type;
        typedef fc::ripemd160               block_id_type;
        typedef fc::sha256                  digest_type;
        typedef uint64_t                    account_name;
        typedef fc::ecc::compact_signature  signature_type;
        typedef std::vector< std::tuple< uint16_t, std::vector<char> > > extendsions_type;
        typedef fc::safe<int64_t>           share_type;

    }//namespace protocol
}//namespace news