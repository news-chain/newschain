//
// Created by boy on 18-6-21.
//


#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>

#include <fc/io/json.hpp>

using namespace news::chain;
using namespace news::base;
int main(){

    try {
        auto var = fc::json::from_string("{\"type\":\"transfer_operation\",\"value\":{\"from\":1,\"to\":2,\"amount\":\"0.00000001, NEWT\",\"memo\":\"memo\"}}");

        transfer_operation tranfers;
        tranfers.from = 1;
        tranfers.to = 2;
        tranfers.amount = asset(NEWS_SYMBOL, 1);
        tranfers.memo = "memo";

        operation op = tranfers;
        idump((op));


        operation tt = var.as<operation>();
        idump((tt));


    }catch (const fc::exception &e){
        ilog("${e}", ("e", e.to_detail_string()));
    }




    return 0;
}