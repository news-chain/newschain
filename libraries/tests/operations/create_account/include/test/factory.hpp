//
// Created by boy on 18-6-26.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>

#include <news/chain/global_property_object.hpp>
#include <news/chain/block_header.hpp>
namespace factory{

    class helper;
    using namespace news::chain;
    using namespace news::base;

    enum  producer_type{
        create_accounts = 1,
        create_transfer,
        create_transfers
    };


    typedef std::function<void(std::vector<signed_transaction>)>    produce_data;







    class helper{
    public:
        helper(){}
        signed_transaction create_account(private_key_type sign_pk, account_name creator, account_name name);
        signed_transaction create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names);
        signed_transaction create_transfer(private_key_type sign_pk, account_name from, account_name to, asset amount);

        std::string     get_string_dynamic_property();


        dynamic_global_property_object  property_object;
    };



    class create_factory{
    public:
        create_factory(){}
        ~create_factory();
        create_factory(producer_type type, int threads = 1, uint32_t trx_op = 1,uint32_t max_cache_trx = 50000);
        void update_param(producer_type type, int threads = 1, uint32_t trx_op = 1,uint32_t max_cache_trx = 50000);

//        std::vector<signed_transaction> get_trx(uint32_t  num);
        void update_dynamic_property(dynamic_global_property_object property_object);

        void            set_producer_data_call(produce_data cb);
        void            start();


    private:
        producer_type           _type;
        produce_data            _cb;
        uint32_t                _trx_op;
        uint32_t                _max_cache;

        helper                  _help;

    };





    std::string  string_json_rpc(const std::string &str);

}