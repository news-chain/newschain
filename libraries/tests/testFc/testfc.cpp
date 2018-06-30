//
// Created by boy on 18-6-16.
//
#include <news/base/asset.hpp>
#include <news/base/asset_symbol.hpp>
#include <fc/io/json.hpp>
#include <news/chain/transaction.hpp>
#include <news/base/types_fwd.hpp>
#include <news/base/fixed_string.hpp>

#include <news/chain/block_header.hpp>
using namespace news::base;
using namespace news::chain;



struct stu{
    fixed_string<16> name;
    int _id;
};

FC_REFLECT(stu, (name)(_id));



int main(){
    block_header hh;
    ilog("${t}", hh);

    return 0;
}