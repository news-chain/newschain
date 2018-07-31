//
// Created by boy on 18-6-26.
//



#include <test/factory.hpp>
#include <thread>
#include <chrono>
namespace factory{

    signed_transaction helper::create_account(news::base::private_key_type sign_pk, news::base::account_name creator,
                                              news::base::account_name name) {
        signed_transaction trx;


        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
        cao.posting = {fc::ecc::private_key::generate().get_public_key(),1};
        cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t)(property_object.head_block_num & 0xffff);

        trx.operations.push_back(cao);
        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }

    signed_transaction
    helper::create_transfer(private_key_type sign_pk, account_name from, account_name to, asset amount) {
            signed_transaction trx;

            transfer_operation transfer;
            transfer.from = from;
            transfer.to = to;
            transfer.amount = amount;
            trx.operations.push_back(transfer);

            trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
            trx.sign(sign_pk, NEWS_CHAIN_ID);

            return trx;
    }

    std::string helper::get_string_dynamic_property() {
        return "{\"jsonrpc\":\"2.0\",\"params\":[\"database_api\",\"get_dynamic_global_property\", {}],\"id\":200000,\"method\":\"call\"} ";
    }

    signed_transaction
    helper::create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names) {
        signed_transaction trx;
        for(auto &name : names){
            create_account_operation cao;
            cao.name = name;
            cao.creator = creator;
            cao.posting = {fc::ecc::private_key::generate().get_public_key(),1};
            cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};
            trx.operations.push_back(cao);
        }

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + 300));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t)(property_object.head_block_num & 0xffff);


        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }


    std::string  string_json_rpc(const std::string &str){
            std::string ret;
            ret = "{\"jsonrpc\":\"2.0\",\"params\":[\"network_broadcast_api\",\"broadcast_transaction\",{\"trx\":" + str +"}],\"id\":-1,\"method\":\"call\"}";
            return ret;
    }




    /*
     *                          create_factory
     *
     * */




    void create_factory::start() {


        int64_t  name = 20;
        bool running = true;

        fc::time_point start = fc::time_point::now();


        while(running){

            std::vector<account_name > names;
            std::vector<signed_transaction> result;

            for(int i = 0; i < _max_cache; i++){

                switch (_type){
                    case producer_type::create_accounts :{
                        for(int i = 0; i < _trx_op; i++){
                            names.push_back(name);
                            name++;
                        }
                        auto trx = _help.create_accounts(NEWS_INIT_PRIVATE_KEY, 1, names);
                        result.push_back(trx);
                        break;
                    }
                    case producer_type::create_transfer :{
                        elog("todo");
                        break;
                    }
                    case producer_type::create_transfers :{
                        //TODO
                        break;
                    }
                    default:{
                        elog("unkown type.");
                    }

                }

                names.clear();
            }
            if(_cb){
                _cb(result);
            }
            result.clear();


            if((fc::time_point::now() - start) < fc::seconds(1)){
                while((fc::time_point::now() - start).count() > fc::seconds(1).count() - 500);
            }


        }
    }

    void create_factory::set_producer_data_call(produce_data cb) {
        _cb = cb;
    }

    create_factory::create_factory(producer_type type, int threads, uint32_t trx_ops, uint32_t max_cache_trx):_type(type),_trx_op(trx_ops), _max_cache(max_cache_trx) {

    }

    void create_factory::update_param(producer_type type, int threads, uint32_t trx_op, uint32_t max_cache_trx) {
        _type = type;
        _trx_op = trx_op;
        _max_cache = max_cache_trx;
    }

    create_factory::~create_factory() {
        elog("~create_factory()");
    }

    void create_factory::update_dynamic_property(dynamic_global_property_object obj) {
        _help.property_object = std::move(obj);
    }


}