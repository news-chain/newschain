//
// Created by boy on 18-6-26.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>
#include <mutex>
namespace factory{


    using namespace news::chain;
    using namespace news::base;

    class helper{
    public: 
		signed_transaction  create_account(news::base::private_key_type& sign_pk,const news::base::account_name& creator,
			const news::base::account_name& name, news::base::private_key_type& genprivate);
        signed_transaction create_transfer(uint64_t taskid,private_key_type& sign_pk, const account_name& from, const account_name& to, asset& amount);
 
	 
    };
	  
    std::string  string_json_rpc(uint64_t taskid, const std::string &str);

}