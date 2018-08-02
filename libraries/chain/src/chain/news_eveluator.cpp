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
            const auto &from = _db.get_account(o.from);
            asset sum(NEWS_SYMBOL, 0);
            for(const auto &itr : o.to_names){
                _db.get_account(itr.first);
                sum += itr.second;
            }
            FC_ASSERT(from.balance >= sum, "${u} has not enough balance.", ("u", o.from));

            _db.modify(from, [&](account_object &obj){
                obj.balance -= sum;
            });

            for(const auto &itr : o.to_names){
                _db.modify(_db.get_account(itr.first), [&](account_object &obj){
                    obj.balance += itr.second;
                });
            }

        }




    }//news::chain
}//news