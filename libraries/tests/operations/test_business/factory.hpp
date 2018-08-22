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
		signed_transaction  create_account(news::base::private_key_type& sign_pk,const news::base::account_name& creator,const news::base::account_name& name, news::base::private_key_type& genprivate);
        signed_transaction create_transfer(uint64_t taskid,private_key_type& sign_pk, const account_name& from, const account_name& to, asset& amount);
		signed_transaction create_transfers(uint64_t taskid, private_key_type& sign_pk, const account_name& from, std::map<account_name, asset> maplist);
		signed_transaction publish_comment(uint64_t taskid, private_key_type& sign_pk,	account_name   author,	std::string    title,		std::string    body,		std::string    permlink,	std::string    metajson	);
		signed_transaction comment_vote(uint64_t taskid, private_key_type& sign_pk,	account_name   vote, account_name author, std::string    permlink, int ticks);
	 
    };
	  
    std::string  string_json_rpc(uint64_t taskid, const std::string &str);

}