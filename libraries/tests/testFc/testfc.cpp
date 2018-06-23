//
// Created by boy on 18-6-16.
//
#include <news/chain/transaction.hpp>
#include <news/base/operation.hpp>


using namespace news::chain;
int main(){

    news::base::create_account_operation cop;
    cop.creator = 1;
    cop.name = 2;





    signed_transaction trx;
    trx.ref_block_prefix = 12;
    trx.ref_block_num = 20;
    trx.operations.push_back(cop);
    ilog("${t}", ("t", trx));


    return 0;
}