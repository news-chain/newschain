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
#include <news/base/types.hpp>

namespace news{
    namespace base{


        struct asset {
            asset(){}
            asset(const asset_symbol &_symbol, const int64_t amount = 0):amount(amount), symbol(_symbol){}

            uint8_t decimals()const{
                return symbol.decimals();
            }

            uint64_t precision()const{
                return symbol.precision();
            }


            friend asset operator + (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol, "symbol invalid.");
                return asset(a.symbol, a.amount + b.amount);
            }

            friend asset operator - (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol, "symbol invalid.");
                return asset(a.symbol, a.amount - b.amount);
            }

            friend bool operator > (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol);
                return a.amount > b.amount;
            }

            friend bool operator >= (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol);
                return a.amount >= b.amount;
            }

            friend bool operator < (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol);
                return a.amount < b.amount;
            }

            friend bool operator <= (const asset &a, const asset &b){
                FC_ASSERT(a.symbol == b.symbol);
                return a.amount <= b.amount;
            }

            asset& operator +=(const asset &a){
                FC_ASSERT(symbol == a.symbol);
                amount += a.amount;
                return *this;
            }

            asset&operator -=(const asset &a){
                FC_ASSERT(symbol == a.symbol);
                amount -= a.amount;
                return *this;
            }

            asset operator -()const { return asset(symbol, -amount); }

            std::string to_string()const;
            static asset from_string(const std::string &str);

            int64_t amount = 0;
            asset_symbol symbol;
        };


    }//namespace base
}//namespace news

namespace fc{
    void to_variant(const news::base::asset &a, fc::variant &v);
    void from_vaiant(const fc::variant &v, news::base::asset &a);


}


FC_REFLECT(news::base::asset, (amount)(symbol))

