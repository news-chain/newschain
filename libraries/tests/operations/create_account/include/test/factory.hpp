//
// Created by boy on 18-6-26.
//

#pragma once

#include <news/chain/transaction.hpp>
#include <news/base/types.hpp>
#include <news/base/operation.hpp>

#include <news/chain/global_property_object.hpp>
#include <news/chain/block_header.hpp>
#include <thread>
#include <fc/crypto/pke.hpp>
#include "types.hpp"

namespace factory{

    class helper;
    using namespace news::chain;
    using namespace news::base;
    using  namespace tools;



    typedef std::function<void(std::vector<signed_transaction>)>    produce_data;


    struct transfer_ops{
        private_key_type    pk;
        account_name        from;
        account_name        to;
        asset               amount;
    };





    class helper{
    public:
        helper(){}
        signed_transaction  create_account(private_key_type sign_pk, account_name creator, account_name name);
        signed_transaction  create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names);
        signed_transaction  create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names, std::map<account_name, fc::ecc::private_key> &map);
        signed_transaction  create_transfer(private_key_type sign_pk, account_name from, account_name to, asset amount);
        signed_transaction  create_ops_transfer(std::vector<transfer_ops> ops);
        signed_transaction  create_transfers(private_key_type sign_pk, account_name from, std::vector<std::map<account_name, asset>> to_asset);
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
        void            stop();

        void            add_test_account(std::map<account_name, fc::ecc::private_key> accounts);

        void            update_data_get_context(const tools::get_context &cxt, bool success);


    private:




    private:
        producer_type                               _type;
        produce_data                                _cb;
        uint32_t                                    _trx_op = 0;
        uint32_t                                    _max_cache = 0;

        helper                                      _help;
        bool                                        _running;
        std::shared_ptr< std::thread >             _thread;

        std::map<account_name, bool> _test_accounts;
    };





    std::string  string_json_rpc(const std::string &str);

}