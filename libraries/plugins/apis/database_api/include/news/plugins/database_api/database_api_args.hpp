//
// Created by boy on 18-6-25.
//

#pragma once

#include <news/chain/global_property_object.hpp>
#include <news/plugins/json_rpc/utility.hpp>

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>

#include <news/base/account_object.hpp>
#include <news/base/asset.hpp>
#include <news/base/comment_object.hpp>
#include <news/chain/transaction.hpp>




using namespace news::chain;
using namespace news::plugins::json_rpc;




namespace news{
    namespace plugins{
        namespace database_api{


            /*
             * dynamic_global_property
             * */
            typedef void_type                       get_dynamic_global_property_args;
            typedef dynamic_global_property_object  get_dynamic_global_property_return;


            struct account{
                account(){}
                account(const news::base::account_object &obj)
                :name(obj.name)
                        ,creator(obj.creator)
                        , balance(obj.balance)
                        , create_time(obj.create_time){

                }
                account_name                name;
                account_name                creator;
                news::base::asset           balance;
                fc::time_point              create_time;
            };



            struct get_accounts_args{
                std::set<account_name> accounts;
            };


            struct get_accounts_return{
                std::vector<account>    accounts;
            };


            struct get_account_public_key_args{
                account_name    name;
            };


            struct account_authority
            {
                account_authority(const account_authority_object &obj)
                :name(obj.name)
                ,posting(obj.posting)
                ,owner(obj.owner){}
                account_name                    name;
                shared_authority                posting;
                shared_authority                owner;
            };

            struct get_account_public_key_return{
                bool            success;
                fc::optional<account_authority> data;
            };


            struct get_transactions_hex_args{
                news::chain::signed_transaction trx;
            };

            struct get_transactions_hex_return{
//                get_transactions_hex_return(std::string &_hash):hash(_hash){}
                std::string hash;
            };


            struct get_transaction_args{
                news::base::transaction_id_type     id;
            };

            typedef news::chain::deatil_signed_transaction  get_transaction_return;



            /*
             *              comment
             * */

            struct comment{
                comment():
                id(0)
                ,author(0)
                ,parent_author(0)
                ,up(0)
                ,down(0){}
                comment(const news::base::comment_object &obj)
                :id(obj.id._id)
                ,author(obj.author)
                ,parent_author(obj.author)
                ,up(obj.up)
                ,down(obj.down)
                ,create_time(obj.create_time){
                    news::base::to_string(obj.title, title);
                    news::base::to_string(obj.body, body);
                    news::base::to_string(obj.permlink, permlink);
                    news::base::to_string(obj.metajson, metajson);
                    news::base::to_string(obj.parent_permlink, parent_permlink);
                }
                int64_t                         id;
                account_name                    author;
                std::string                     title;
                std::string                     body;
                std::string                     permlink;
                std::string                     metajson;
                account_name                    parent_author;
                std::string                     parent_permlink;
                uint64_t                        up;
                uint64_t                        down;
                fc::time_point                  create_time;
            };


            struct get_comment_by_id_args{
                int64_t         id;
            };

            struct get_comment_by_id_return{
                bool                        success = false;
                fc::optional<comment>     data;
            };


            struct get_comment_by_permlink_args{
                account_name        author;
                std::string         permlink;
            };

            typedef get_comment_by_id_return    get_comment_by_permlink_return;




        }
    }
}

FC_REFLECT(news::plugins::database_api::account, (name)(creator)(balance)(create_time))
FC_REFLECT(news::plugins::database_api::get_accounts_args, (accounts))
FC_REFLECT(news::plugins::database_api::get_accounts_return, (accounts))

FC_REFLECT(news::plugins::database_api::get_transactions_hex_args, (trx))
FC_REFLECT(news::plugins::database_api::get_transactions_hex_return, (hash))
FC_REFLECT(news::plugins::database_api::get_transaction_args, (id))

FC_REFLECT(news::plugins::database_api::get_comment_by_id_args, (id))
FC_REFLECT(news::plugins::database_api::comment, (id)(author)(title)(body)(permlink)(metajson)(up)(down)(create_time))
FC_REFLECT(news::plugins::database_api::get_comment_by_id_return, (success)(data))
FC_REFLECT(news::plugins::database_api::get_comment_by_permlink_args, (author)(permlink))


FC_REFLECT(news::plugins::database_api::get_account_public_key_args, (name))
FC_REFLECT(news::plugins::database_api::account_authority, (name)(posting)(owner))
FC_REFLECT(news::plugins::database_api::get_account_public_key_return, (success)(data))


