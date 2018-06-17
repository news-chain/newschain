//
// Created by boy on 18-6-11.
//



/*
 *      asset:
 *      decimals    12
 *      symbol      NEWS_SYMBOL
 *
 *
 * */




#pragma once

#include <news/base/asset_symbol.hpp>
namespace news{
    namespace base{


        struct asset {
            asset(const asset &_asset, const asset_symbol &_symbol):amount(_asset.amount), symbol(_symbol){}

            asset(share_type a, asset_symbol _symbol):amount(a), symbol(_symbol){}
            share_type amount;
            asset_symbol symbol;

            asset& operator +(const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol);
                return asset(a.amount + b.amount, a.symbol);
            }

        };



    }//namespace base
}//namespace news

