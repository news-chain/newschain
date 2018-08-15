//
// Created by boy on 18-6-26.
//



#include "factory.hpp"
#include <random>
#include <stdio.h>
namespace factory{

	 
    signed_transaction helper::create_account(news::base::private_key_type& sign_pk,const  news::base::account_name& creator,
                                            const  news::base::account_name& name,news::base::private_key_type& genprivate) { 
		 
        signed_transaction trx;
        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
		genprivate = fc::ecc::private_key::generate();

		cao.posting = { genprivate.get_public_key(),1 };
		cao.owner = { fc::ecc::private_key::generate().get_public_key(), 1 };
        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
        trx.operations.push_back(cao);
        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }


	signed_transaction helper::publish_comment(uint64_t taskid, private_key_type& sign_pk,
		account_name   author, std::string    title, std::string    body, std::string    permlink, std::string    metajson)
	{

		signed_transaction trx;
		comment_operation cao;
		cao.author = author;
		cao.title = title;
		cao.permlink = permlink;
		cao.metajson = metajson;  
		cao.parent_author = 0;
		cao.parent_permlink = "";
		trx.operations.push_back(cao);
		trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 3000));
		trx.sign(sign_pk, NEWS_CHAIN_ID);
		return trx;
	}
	signed_transaction helper::create_transfers(uint64_t taskid, private_key_type& sign_pk, const account_name& from, std::map<account_name, asset> maplist)
	{ 
		signed_transaction trx;
		transfers_operation transfer;
		transfer.from = from;
		transfer.to_names = std::move(maplist);
		fc::variant fc(taskid);
		transfer.memo = fc.as<std::string>();
		trx.operations.push_back(transfer);
		trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
		trx.sign(sign_pk, NEWS_CHAIN_ID);
		return trx;
	}

    signed_transaction
    helper::create_transfer(uint64_t taskid,private_key_type& sign_pk,const account_name& from, const account_name& to, asset& amount) 
	{ 
		
		signed_transaction trx;
            transfer_operation transfer;
            transfer.from = from;
            transfer.to = to;			
			transfer.amount = amount;
			fc::variant fc(taskid);
			transfer.memo = fc.as<std::string>();
            trx.operations.push_back(transfer); 
            trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
            trx.sign(sign_pk, NEWS_CHAIN_ID);  
            return trx;
    }
	signed_transaction helper::comment_vote(uint64_t taskid, private_key_type& sign_pk,
		account_name   vote, std::string    permlink, int ticks)
	{
		signed_transaction trx;
		comment_vote_operation transfer;
		transfer.voter = vote;
		transfer.permlink = permlink; 
		transfer.ticks= ticks;  
		fc::variant fc(taskid);
		transfer.memo = fc.as<std::string>();
		trx.operations.push_back(transfer);
		trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
		trx.sign(sign_pk, NEWS_CHAIN_ID);
		return trx;

	}

    std::string  string_json_rpc(uint64_t taskid,const std::string &str)
	{ 
		   fc::variant vv(taskid);
		   std::string  ret = "{\"jsonrpc\":\"2.0\",\"params\":[\"network_broadcast_api\",\"broadcast_transaction\",{\"trx\":" + str +"}],\"id\":"+vv.as<std::string>()+",\"method\":\"call\"}";
		  return ret;
    }
}