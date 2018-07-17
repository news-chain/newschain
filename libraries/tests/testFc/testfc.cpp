//
// Created by boy on 18-6-16.
//


#include <news/plugins/database_api/database_api_args.hpp>

#include <news/base/types.hpp>

using namespace news::plugins::database_api;
using namespace news::base;
int main(int argc, char **argv) {

    std::cout << "private_key_type " << sizeof(private_key_type) << std::endl;
    std::cout << "digest_type " << sizeof(digest_type) << std::endl;
    std::cout << "checksum_type " << sizeof(checksum_type) << std::endl;
    std::cout << "chain_id_type " << sizeof(chain_id_type) << std::endl;
    std::cout << "transaction_id_type " << sizeof(transaction_id_type) << std::endl;
    std::cout << "block_id_type " << sizeof(block_id_type) << std::endl;
    std::cout << "digest_type " << sizeof(digest_type) << std::endl;
    std::cout << "account_name " << sizeof(account_name) << std::endl;
    std::cout << "digest_type " << sizeof(signature_type) << std::endl;
    std::cout << "share_type " << sizeof(share_type) << std::endl;
    std::cout << "buffer_type " << sizeof(buffer_type) << std::endl;
    std::cout << "time_point_sec " << sizeof(fc::time_point_sec) << std::endl;




    return 0;
}