//
// Created by boy on 18-6-26.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>
namespace factory{


    using namespace news::chain;
    using namespace news::base;

    class helper{
    public:
		signed_transaction & helper::create_account(news::base::private_key_type& sign_pk, news::base::account_name& creator,
			news::base::account_name& name, news::base::private_key_type& genprivate);
        signed_transaction& create_transfer(private_key_type& sign_pk, account_name& from, account_name& to, asset& amount);
    };
	  
    std::string&  string_json_rpc(const std::string &str);

}