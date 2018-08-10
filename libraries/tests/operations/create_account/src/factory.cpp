//
// Created by boy on 18-6-26.
//



#include <test/factory.hpp>
#include <thread>
#include <chrono>
#include <boost/random.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#define EXPIRATION_TIME     180



namespace factory {

    signed_transaction helper::create_account(news::base::private_key_type sign_pk, news::base::account_name creator,
                                              news::base::account_name name) {
        signed_transaction trx;


        create_account_operation cao;
        cao.name = name;
        cao.creator = creator;
        cao.posting = {fc::ecc::private_key::generate().get_public_key(), 1};
        cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + EXPIRATION_TIME));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t) (property_object.head_block_num & 0xffff);

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
        transfer.memo = boost::uuids::to_string(boost::uuids::random_generator()());
        trx.operations.push_back(transfer);

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + EXPIRATION_TIME));
        trx.sign(sign_pk, NEWS_CHAIN_ID);

        return trx;
    }

    std::string helper::get_string_dynamic_property() {
        return "{\"jsonrpc\":\"2.0\",\"params\":[\"database_api\",\"get_dynamic_global_property\", {}],\"id\":200000,\"method\":\"call\"} ";
    }

    signed_transaction
    helper::create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names) {
        signed_transaction trx;
        for (auto &name : names) {
            create_account_operation cao;
            cao.name = name;
            cao.creator = creator;
            cao.posting = {fc::ecc::private_key::generate().get_public_key(), 1};
            cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};
            trx.operations.push_back(cao);
        }

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + EXPIRATION_TIME));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t) (property_object.head_block_num & 0xffff);


        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;
    }

    signed_transaction
    helper::create_accounts(private_key_type sign_pk, account_name creator, const std::vector<account_name> &names,
                            std::map<account_name, fc::ecc::private_key> &map) {

        signed_transaction trx;
        for (auto &name : names) {
            auto post_key = fc::ecc::private_key::generate();
            create_account_operation cao;
            cao.name = name;
            cao.creator = creator;
            cao.posting = {post_key.get_public_key(), 1};
            cao.owner = {fc::ecc::private_key::generate().get_public_key(), 1};
            trx.operations.emplace_back(std::move(cao));

            map[name] = post_key;
        }

        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + EXPIRATION_TIME));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t) (property_object.head_block_num & 0xffff);


        trx.sign(sign_pk, NEWS_CHAIN_ID);
        return trx;

    }

    signed_transaction
    helper::create_transfers(private_key_type sign_pk, account_name from,
                             std::vector<std::map<account_name, asset>> asset) {

        signed_transaction trx;
        for (auto &itr : asset) {
            transfers_operation to;
            to.from = from;
            to.to_names = std::move(itr);
            to.memo = boost::uuids::to_string(boost::uuids::random_generator()());
            trx.operations.push_back(to);
        }


        trx.set_expiration(fc::time_point_sec(fc::time_point::now().sec_since_epoch() + EXPIRATION_TIME));
        trx.ref_block_prefix = property_object.head_block_id._hash[1];
        trx.ref_block_num = (uint16_t) (property_object.head_block_num & 0xffff);
        trx.sign(sign_pk, NEWS_CHAIN_ID);

        return trx;
    }


    std::string string_json_rpc(const std::string &str) {
        std::string ret;
        ret = "{\"jsonrpc\":\"2.0\",\"params\":[\"network_broadcast_api\",\"broadcast_transaction\",{\"trx\":" + str +
              "}],\"id\":-1,\"method\":\"call\"}";
        return ret;
    }


    /*
     *                          create_factory
     *
     * */




    void create_factory::start() {
        _thread = std::make_shared<std::thread>([&]() {
//            int64_t name = 20;
            fc::time_point start = fc::time_point::now();
            boost::mt19937 rng(time(0));

            std::vector<account_name> transfer_names;
            std::map<account_name, fc::ecc::private_key> names_pk;
            std::map<account_name, int64_t> names_balance;
            size_t max_transfer_accounts_size = 200;

            int sleep_second = 3;
            while (_running) {
                std::vector<account_name> names;
                std::vector<signed_transaction> result;

                if (sleep_second < 0) {


                    for (size_t i = 0; i < _max_cache; i++) {
                        signed_transaction trx;

                        //第一次，创建账号
                        if (transfer_names.size() < _max_cache && _type == producer_type::create_transfer) {
                            for (int i = 0; i < _max_cache; i++) {
                                transfer_names.push_back(rng());
                            }
                            trx = _help.create_accounts(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, transfer_names,
                                                        names_pk);
                            result.push_back(trx);
                            break;
                        }

                        if (transfer_names.size() < max_transfer_accounts_size &&
                            _type == producer_type::create_transfers) {
                            for (int i = 0; i < max_transfer_accounts_size; i++) {
                                transfer_names.push_back(rng());
                            }
                            trx = _help.create_accounts(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, transfer_names,
                                                        names_pk);
                            result.push_back(trx);
                            break;
                        }

                        switch (_type) {
                            case producer_type::create_accounts : {
                                for (size_t j = 0; j < _trx_op; j++) {
                                    names.push_back(rng());
                                }
                                trx = _help.create_accounts(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, names);
                                result.push_back(trx);
                                break;
                            }
                            case producer_type::create_transfer : {

                                uint64_t random = rng() % transfer_names.size();
                                account_name op_name = transfer_names[random];

                                if (names_balance[op_name] <= 100) {
                                    trx = _help.create_transfer(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME,
                                                                op_name, asset(NEWS_SYMBOL, 1));
                                    names_balance[op_name] += 1;
                                } else {
                                    trx = _help.create_transfer(names_pk[op_name], op_name, NEWS_SYSTEM_ACCOUNT_NAME,
                                                                asset(NEWS_SYMBOL, 1));
                                    names_balance[op_name] -= 1;
                                }
                                result.push_back(trx);


                                break;
                            }
                            case producer_type::create_transfers : {
//                            i = _max_cache; //break for
                                std::map<account_name, asset> tf_map;
                                for (auto &itr : transfer_names) {
                                    tf_map[itr] = asset(NEWS_SYMBOL, 1);
                                }

                                std::vector<std::map<account_name, asset>> assets;
                                for (int j = 0; j < _trx_op; j++) {
                                    assets.push_back(tf_map);
                                }
                                trx = _help.create_transfers(NEWS_INIT_PRIVATE_KEY, NEWS_SYSTEM_ACCOUNT_NAME, assets);
                                result.push_back(trx);
                                break;
                            }
                            default: {
                                elog("unkown type.");
                                assert(false);
                            }

                        }

                        names.clear();
                    }
                }
                else{
                    sleep_second--;
                }
                if (_cb) {
                    _cb(result);
                }


                result.clear();

                if ((fc::time_point::now() - start) < fc::seconds(1)) {
                    int64_t sl = (start + fc::seconds(1) - fc::time_point::now()).count() / 1000;
                    if (sl != 0) {
                        std::this_thread::sleep_for(std::chrono::operator ""ms(sl));
                    }
                }
                start = fc::time_point::now();

            } // while
        });
    }

    void create_factory::set_producer_data_call(produce_data cb) {
        _cb = cb;
    }

    create_factory::create_factory(producer_type type, int threads, uint32_t trx_ops, uint32_t max_cache_trx) : _type(
            type), _trx_op(trx_ops), _max_cache(max_cache_trx), _running(true) {

    }

    void create_factory::update_param(producer_type type, int threads, uint32_t trx_op, uint32_t max_cache_trx) {
        _type = type;
        _trx_op = trx_op;
        _max_cache = max_cache_trx;
        _running = true;
    }

    create_factory::~create_factory() {
        elog("~create_factory()");
    }

    void create_factory::update_dynamic_property(dynamic_global_property_object obj) {
        _help.property_object = std::move(obj);
    }

    void create_factory::stop() {
//        _running = false;

    }


}