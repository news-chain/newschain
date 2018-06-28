//
// Created by boy on 18-6-16.
//
#include <news/base/asset.hpp>
#include <news/base/asset_symbol.hpp>
#include <fc/io/json.hpp>
#include <news/chain/transaction.hpp>


using namespace news::base;
using namespace news::chain;
int main(){
    signed_transaction trx;

    transfer_operation to;
    to.from = 1;
    to.to = 1;
    to.amount = asset(NEWS_SYMBOL, 120);

    trx.operations.push_back(to);
    trx.set_expiration(fc::time_point::now());

    private_key_type pk = fc::ecc::private_key::generate();
    trx.sign(pk, NEWS_CHAIN_ID);

    asset a(NEWS_SYMBOL, 1);
    ilog("${a}", ("a",a));
    idump((trx));

    return 0;
}