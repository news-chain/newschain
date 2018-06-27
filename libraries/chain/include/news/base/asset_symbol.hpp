//
// Created by boy on 18-6-12.
//

#pragma once


#include <string>

#include <news/base/config.hpp>
#include <news/base/types.hpp>
namespace news{
    namespace base{






        struct asset_symbol{
            asset_symbol(uint64_t value = NEWS_SYMBOL):_symbol(value){
            }

            asset_symbol(std::string name, uint8_t decimals){
                FC_ASSERT(name.length(), "asset_symbol.name.length too long.");

            }

            bool operator == (const asset_symbol &a) const{
                return a._symbol == _symbol;
            }

            std::string name() const{
                uint64_t value = _symbol;
                value >>= 8;
                std::string ret;
                while(value > 0){
                    char c = value & 0xFF;
                    ret += c;
                    value >>= 8;
                }
                return ret;
            }

            std::string to_string()const{
                return std::string();
            }

            uint8_t decimals()const{
                return (uint8_t)_symbol&0xFF;
            }

            uint64_t precision()const{
                uint64_t p10 = 1;
                uint64_t p = decimals();
                while(p > 0){
                    p10 *= 10;
                    --p;
                }
                return p10;
            }


            static asset_symbol from_string(const std::string &name);


            uint64_t _symbol = 0;
        };



    }//namespace base
}//namespace news


namespace  fc{
//    void to_variant(const news::base::asset_symbol &a, fc::variant &v);
//    void from_variant(const fc::variant &v, news::base::asset_symbol &a);
}



FC_REFLECT(news::base::asset_symbol, (_symbol) )


