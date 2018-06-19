//
// Created by boy on 18-6-11.
//

#pragma once



#include <fc/crypto/sha256.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/exception/exception.hpp>
#include <fc/crypto/base58.hpp>
#include <fc/io/raw.hpp>

#include <news/base/config.hpp>

#include <news/base/version.hpp>

#include <boost/container/flat_set.hpp>






namespace news{
    namespace base{


        struct void_t{};
        typedef fc::static_variant<void_t, version>     block_header_extensions;
        typedef boost::container::flat_set<block_header_extensions>   block_header_extenions_type;






        typedef fc::sha256                  digest_type;
        typedef fc::sha256                  checksum_type;
        typedef fc::sha256                  chain_id_type;
        typedef fc::ripemd160               transaction_id_type;
        typedef fc::ripemd160               block_id_type;
        typedef fc::sha256                  digest_type;
        typedef uint64_t                    account_name;
        typedef fc::ecc::compact_signature  signature_type;

        typedef fc::safe<int64_t>           share_type;





        struct public_key_type
        {
            struct binary_key
            {
                binary_key() {}
                fc::ecc::public_key_data data;
                uint32_t                 check = 0;
            };
            fc::ecc::public_key_data key_data;
            public_key_type();
            public_key_type( const fc::ecc::public_key_data& data );
            public_key_type( const fc::ecc::public_key& pubkey );
            explicit public_key_type( const std::string& base58str );
            operator fc::ecc::public_key_data() const;
            operator fc::ecc::public_key() const;
            explicit operator std::string() const;
            friend bool operator == ( const public_key_type& p1, const fc::ecc::public_key& p2);
            friend bool operator == ( const public_key_type& p1, const public_key_type& p2);
            friend bool operator < ( const public_key_type& p1, const public_key_type& p2) { return p1.key_data < p2.key_data; }
            friend bool operator != ( const public_key_type& p1, const public_key_type& p2);
        };





    }//namespace base
}//namespace news


namespace fc{
    void to_variant(const news::base::public_key_type &var, fc::variant &vo);
    void from_variant(const fc::variant &var, news::base::public_key_type &vo);
}



FC_REFLECT( news::base::public_key_type::binary_key, (data)(check) )
FC_REFLECT( news::base::public_key_type, (key_data) )
FC_REFLECT_TYPENAME(news::base::share_type)