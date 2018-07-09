//
// Created by boy on 18-6-13.
//

#pragma once

#include <string>
#include <fc/optional.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/base58.hpp>

namespace news{
    namespace base{


        std::string key_to_wif(const fc::sha256 &private_secret);

        std::string key_to_wif(const fc::ecc::private_key &pk);

        fc::optional<fc::ecc::private_key> wif_to_key(const std::string &wif_key);

    }//namespace base
}//namespace news
