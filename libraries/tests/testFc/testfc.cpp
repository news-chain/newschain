//
// Created by boy on 18-6-16.
//
#include <news/base/asset.hpp>
#include <news/base/asset_symbol.hpp>
#include <fc/io/json.hpp>
#include <news/chain/transaction.hpp>
#include <news/base/types_fwd.hpp>

#include <news/chain/block_header.hpp>



struct stu{
    news::base::fixed_string<16> name;
    int _id;
};

FC_REFLECT(stu, (name)(_id));



int main(){
    stu tt;
    ilog("${t}", ("t", tt));

    return 0;
}