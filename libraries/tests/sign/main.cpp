//
// Created by boy on 18-6-21.
//


#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>


using namespace news::chain;
using namespace news::base;
int main(){

    chain_id_type chain_id = fc::sha256::hash("123");
    private_key_type private_key = private_key_type::generate();
    public_key_type public_key = private_key.get_public_key();

    signed_transaction trx;
    trx.set_expiration(fc::time_point::now());
    create_account_operation op;
    op.public_key = public_key;
    op.creator = 1;
    op.name = 2;

    trx.operations.push_back(op);



    private_key_type private_key1 = private_key_type::generate();
    public_key_type public_key1 = private_key1.get_public_key();

    transfer_operation top;
    top.from = 1;
    top.to = 1;
    top.memo = "12312312";
    ilog("public1 ${p}", ("p", public_key1));

    trx.operations.push_back(top);




    trx.sign(private_key, chain_id);
    trx.sign(private_key1, chain_id);
    elog("${t}",("t", trx));
    auto pkeys = trx.get_signature_keys(chain_id);
    for(auto &k : pkeys){
        ilog("${k}", ("k", k));
    }







    return 0;
}