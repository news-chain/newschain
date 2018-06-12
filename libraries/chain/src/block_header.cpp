//
// Created by boy on 18-6-11.
//


#include <news/chain/block_header.hpp>



namespace news{
    namespace chain{


        digest_type block_header::digest() const {
            return digest_type::hash(*this);
        }


        block_id_type signed_block_header::id() const {
            auto tmp = fc::sha224::hash(*this);
            tmp._hash[0] = fc::endian_reverse_u32(block_num());
            static_assert(sizeof( tmp._hash[0]) == 4, "should be 4 bytes");
            block_id_type ret;
            memcpy(ret._hash, tmp._hash, std::min(sizeof(ret), sizeof(tmp)) );
            return ret;
        }

        fc::ecc::public_key signed_block_header::signee() const {
            return fc::ecc::public_key( producer_signature, digest(), true);
        }


        void signed_block_header::sign(const fc::ecc::private_key &signer) {
            producer_signature = signer.sign_compact(digest());
        }

        bool signed_block_header::validate_signee(const fc::ecc::public_key &expected_signee) const {
            return signee() == expected_signee;
        }

        uint32_t block_header::num_from_id(const news::protocol::block_id_type &id) {
            return fc::endian_reverse_u32(id._hash[0]);
        }



    }//namespace chain
}//namesapce news