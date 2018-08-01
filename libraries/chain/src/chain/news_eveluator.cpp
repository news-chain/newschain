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
            });

            _db.create<account_authority_object>([&](account_authority_object &obj){
                obj.name = o.name;
                obj.posting = o.posting;
                obj.owner = o.owner;
            });

        }

        void transfer_evaluator::do_apply(const transfer_operation &o)
        {
            const auto &from = _db.get_account(o.from);
            const auto &to = _db.get_account(o.to);
            FC_ASSERT(from.balance.amount >= o.amount.amount, "${from} not enough balance");
            _db.modify(from , [&](account_object &obj){
                obj.balance.amount -= o.amount.amount;
            });

            _db.modify(to, [&](account_object &obj){
                obj.balance.amount += o.amount.amount;
            });
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




    }//news::chain
}//news