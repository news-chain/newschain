//
// Created by boy on 18-6-19.
//

#include <news/chain/news_eveluator.hpp>
#include <news/base/account_object.hpp>
#include <chainbase/chainbase.hpp>
#include <news/chain/database.hpp>
namespace news{
    namespace chain{

        using namespace news::base;
        using namespace chainbase;


        void create_account_evaluator::do_apply(const create_account_operation &o) {
            const auto &itr = _db.get_index<account_object_index>().indices().get<by_name>();
            FC_ASSERT(itr.find(o.creator) != itr.end(), "not find creator ${c}", ("c", o.creator));
            FC_ASSERT(itr.find(o.name) == itr.end(), "user ${n} is existed.", ("n", o.name));

            _db.create<account_object>([&](account_object &obj){
                obj.name = o.name;
                obj.creator = o.creator;
                obj.create_time = _db.head_block_time();
                to_shared_string((std::string)o.public_key, obj.public_key);
            });
        }

        void transfer_evaluator::do_apply(const transfer_operation &o)
        {
            const auto &it = _db.get_index<account_object_index>().indices().get<by_name>();
            FC_ASSERT(it.find(o.from) != it.end(), "not find sender ${from}", ("from", o.from));
            FC_ASSERT(it.find(o.to) != it.end(), "not find receiver ${to}", ("to", o.to));

            FC_ASSERT(o.amount > asset(o.amount.symbol, 0), "sender's amount must be greater than nought ${a}", ("a", o.amount));

            const account_object *ac = _db.find_account(o.from);
            FC_ASSERT( _db.get_balance( *ac, o.amount.symbol ) >= o.amount, "Account does not have sufficient funds for transfer." );
            _db.adjust_balance( *ac, -o.amount );
            ac = _db.find_account(o.to);
            _db.adjust_balance( *ac, o.amount );
        }

        void transfers_evaluator::do_apply(const news::base::transfers_operation &o)
        {
            //check out the existence of sender
            const auto &it = _db.get_index<account_object_index>().indices().get<by_name>();
            FC_ASSERT(it.find(o.from) != it.end(), "not find sender ${from}", ("from", o.from));
            FC_ASSERT(o.to_names.size() >= 1, "number of receivers must be greater than 0");
            const auto &r = o.to_names.begin();
            std::string str;
            asset ar(r->second.symbol, 0);
            for(const auto& r : o.to_names)
            {
//                str = r.second.to_string();

                //check out the existence of each of receivers
                FC_ASSERT(it.find(r.first) != it.end(), "not find receiver: ${r}", ("r", r.first));
                //summary every payments
                ar += r.second;
            }

            const account_object *ac = _db.find_account(o.from);
            FC_ASSERT( _db.get_balance( *ac, ar.symbol ) >= ar, "Account does not have sufficient funds for transfer." );
            _db.adjust_balance( *ac, -ar );

            for(const auto& r : o.to_names)
            {
                ac = _db.find_account(r.first);
                _db.adjust_balance( *ac, r.second );
            }
        }



		void comment_evaluator::do_apply(const news::base::comment_operation &o)
		{    
			const auto &it = _db.get_index<comment_object_index>().indices().get<by_permlink>(); 
 
			auto iffind = it.find(o.permlink);
			if (iffind != it.end()) 
				FC_ASSERT(false,"have find permlink: ${r}", ("r", o.permlink));   
			_db.create<comment_object>([&](comment_object &obj) {
				obj.author = o.author;
				obj.up = obj.down = 0;
				to_shared_string(o.title, obj.title);
				to_shared_string(o.body, obj.body);
				to_shared_string(o.permlink, obj.permlink);
				to_shared_string(o.metajson, obj.metajson); 
				obj.create_time = _db.head_block_time(); 			 
			});  
			 
			 
		} 

		void comment_vote_evaluator::do_apply(const news::base::comment_vote_operation &o)
		{
			const auto &it = _db.get_index<comment_object_index>().indices().get<by_permlink>();
			auto iffind = it.find(o.permlink);
			if (iffind != it.end())
				FC_ASSERT(false, "have find permlink: ${r}", ("r", o.permlink));  
			if(o.ticks==0)
				FC_ASSERT(false, "ticks ==0 "); 
			if(o.ticks>0)
			_db.modify(*iffind, [&](comment_object &obj) {
				obj.up+=o.ticks;
			});  
			else{
				_db.modify(*iffind, [&](comment_object &obj) {
					obj.down += o.ticks;
				});
				}
			_db.create<comment_vote_object>([&](comment_vote_object &obj) {
				obj.voter = o.voter;
				obj.ticks = o.ticks;
				to_shared_string(o.permlink, obj.permlink); 
				obj.create_time = _db.head_block_time();
			});
		}


        void packed_block_reward_evaluator::do_apply(const news::base::packed_block_reward_operation &op){
            FC_ASSERT(op.to_name == NEWS_SYSTEM_ACCEPT_NAME, "only system account");
            FC_ASSERT(op.producer == _db.get_scheduled_producer(_db.head_block_num() + 1), "packed producer error");
            const auto &to = _db.get_account(op.to_name);
            _db.modify(to, [&](account_object &obj){
                obj.balance += op.reward;
            });
        }

    }//news::chain
}//news