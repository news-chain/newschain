//
// Created by boy on 18-6-12.
//



#include <news/plugins/producer_plugin/producer_plugin.hpp>
#include <news/base/key_conversion.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>


namespace news{
    namespace plugins{
        namespace producer_plugin{


            using namespace news::base;

            void new_chain_banner()
            {
                std::cerr << "\n"
                             "*******************************\n"
                             "*                             *\n"
                             "*   ------ NEW CHAIN ------   *\n"
                             "*   -   Welcome to NEWS!  -   *\n"
                             "*   -----------------------   *\n"
                             "*                             *\n"
                             "*******************************\n"
                             "\n";
                return;
            }


            namespace detail{
                class producer_plugin_impl{
                public:
                    producer_plugin_impl(boost::asio::io_service &io):_timer(io),
                                                                      _chain_plugin(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>()),
                                                                      _db(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>().get_database()){}
                    ~producer_plugin_impl(){}

                    void schedule_production_loop();
                    block_production_condition::block_production_condition_enum  block_production_loop();
                    block_production_condition::block_production_condition_enum  maybe_produce_block(fc::mutable_variant_object &cap);

                    boost::program_options::variables_map   _options;
                    bool                                    _production_enabled;
                    uint32_t                                _required_producer_participation = 0;
                    uint32_t                                _production_skip_flags;
//                    std::map<news::chain>

                    boost::asio::deadline_timer                                         _timer;

                    std::map<news::base::public_key_type, fc::ecc::private_key>     _private_keys;
                    news::plugins::chain_plugin::chain_plugin&                           _chain_plugin;
                    news::chain::database&                                              _db;
                    std::set< news::base::account_name >                            _producers;

                };


                void producer_plugin_impl::schedule_production_loop() {
                    fc::time_point now = fc::time_point::now();
                    int64_t time_to_next_block_time = 1000000 - (now.time_since_epoch().count() % 1000000);
                    if(time_to_next_block_time < 50000) //50ms
                    {
                        time_to_next_block_time += 1000000;
                    }
                    _timer.expires_from_now(boost::posix_time::microseconds(time_to_next_block_time));
                    _timer.async_wait(boost::bind( &producer_plugin_impl::block_production_loop, this));

                }

                block_production_condition::block_production_condition_enum producer_plugin_impl::maybe_produce_block(
                        fc::mutable_variant_object &cap) {

                    fc::time_point_sec now = fc::time_point::now();
//                    fc::time_point now = now_fine + fc::microseconds(500000l);
                    if(!_production_enabled){
                        if(_db.get_slot_time(1) >= now){
                            _production_enabled = true;
                        }else{
                            return  block_production_condition::not_synced;
                        }
                    }

                    uint32_t  slot = _db.get_slot_at_time(now);
                    if(slot == 0){
                        cap("next time", _db.get_slot_time(1));
                        return block_production_condition::not_time_yet;
                    }

                    //
                    // this assert should not fail, because now <= db.head_block_time()
                    // should have resulted in slot == 0.
                    //
                    // if this assert triggers, there is a serious bug in get_slot_at_time()
                    // which would result in allowing a later block to have a timestamp
                    // less than or equal to the previous block
                    //
                    assert( now > _db.head_block_time() );

                    //TODO account_name_type to string?
                    auto producer = _db.get_scheduled_producer(slot);
                    if(_producers.find(producer) == _producers.end()){
                        cap("scheduled_producer ", producer);
                        return block_production_condition::not_my_turn;
                    }

                    auto scheduled_time = _db.get_slot_time(slot);

                    //TODO find producer public_key  from  witness_object
                    news::base::public_key_type key = _private_keys.begin()->first;
                    auto private_key = _private_keys.find(key);
                    if(private_key == _private_keys.end()){
                        cap("k", (std::string)key);
                        return block_production_condition::no_private_key;
                    }

                    //TODO required_witness_participation ?

                    if(llabs((scheduled_time - now).count()) > fc::microseconds(500).count() ){
                        cap("scheduled_time", scheduled_time)("now", now);
                        return block_production_condition::lag;
                    }

                    //TODO add skip flag
                    auto block = _chain_plugin.generate_block(now, producer, private_key->second, 0);
//                    ilog("block : ${b}", ("b", block));
                    cap("n", block.block_num())("t", block.timestamp)("p", block.producer)("b", block.transactions.size())("size", fc::raw::pack_size(block));
                    //TODO broadcast block

                    return block_production_condition::produced;

                }

                block_production_condition::block_production_condition_enum producer_plugin_impl::block_production_loop() {
                    if(fc::time_point::now() < fc::time_point(NEWS_GENESIS_TIME)){
                        wlog("waiting until genesis to produce block : ${t}", ("t", NEWS_GENESIS_TIME));
                        return block_production_condition::wait_for_genesis;
                    }
                    block_production_condition::block_production_condition_enum  result= block_production_condition::not_know;
                    fc::mutable_variant_object capture;
                    try {
                        result = maybe_produce_block(capture);



                    }
                    catch (const fc::exception &e){
                        elog("generate block ${e}", ("e", e.to_detail_string() ));
                    }
                    catch (...){
                        return block_production_condition::exception_producing_block;
                    }

                    switch (result){
                        case block_production_condition::produced:
                            elog("Genarated block #${n} time ${t} by ${p} trx.size=${b}, pack_size:${size}", (capture));
                        case block_production_condition::not_synced:
//                                     ilog("Not producing block because production is disabled until we receive a recent block (see: --enable-stale-production)");
                            break;
                        case block_production_condition::not_my_turn:
//                                     ilog("Not producing block because it isn't my turn");
                            break;
                        case block_production_condition::not_time_yet:
//                                     ilog("Not producing block because slot has not yet arrived");
                            break;
                        case block_production_condition::no_private_key:
                            ilog("Not producing block because I don't have the private key for ${k}", (capture) );
                            break;
                        case block_production_condition::low_participation:
                            elog("Not producing block because node appears to be on a minority fork with only ${pct}% witness participation", (capture) );
                            break;
                        case block_production_condition::lag:
//                            elog("Not producing block because node didn't wake up within 500ms of the slot time.");
                            break;
                        case block_production_condition::consecutive:
                            elog("Not producing block because the last block was generated by the same witness.\nThis node is probably disconnected from the network so block production has been disabled.\nDisable this check with --allow-consecutive option.");
                            break;
                        case block_production_condition::exception_producing_block:
                            elog( "exception producing block" );
                            break;
                        default:
                            elog("block_production_condition result unhandle");
                    }


                    schedule_production_loop();
                    return result;
                }


            }//namespace detail


            producer_plugin::producer_plugin():_my( new detail::producer_plugin_impl( news::app::application::getInstance().get_io_service() ) ) {

            }


            producer_plugin::~producer_plugin() {

            }

            void producer_plugin::set_program_options(options_description &, options_description &cfg) {


                std::vector<std::string> default_producer = {std::to_string(NEWS_SYSTEM_ACCOUNT_NAME)};
                std::string pdef = boost::algorithm::join(default_producer, "-");

                std::vector<std::string> d_key = {(news::base::key_to_wif(NEWS_INIT_PRIVATE_KEY.get_secret()))};
                std::string d_pkey = boost::algorithm::join(d_key, "-");


                cfg.add_options()
                        ("enable-stale-production", bpo::bool_switch()->default_value(false), "Enable block production, even if the chain is stale.")
                        ("producer-name", bpo::value<std::string>()->composing()->default_value(pdef), "producer name vector")
                        ("private-key", bpo::value<std::string>()->composing()->default_value(d_pkey), "private keys for producer name");


            }

            void producer_plugin::plugin_initialize(const variables_map &options) {
                if(options.count("private-key") && options.count("producer-name")){
                    std::string keys_string = options["private-key"].as<std::string>();
                    std::vector<std::string> keys;
                    boost::split(keys, keys_string, boost::is_any_of("-"));

                    std::string names_string = options["producer-name"].as<std::string>();
                    std::vector<std::string> names;
                    boost::split(names, names_string, boost::is_any_of("-"));

                    int i = 0;
                    for(const std::string &wif_key : keys){
                        fc::optional<fc::ecc::private_key> pk = news::base::wif_to_key(wif_key);
                        FC_ASSERT(pk.valid(), "unable to parse private key");
                        _my->_private_keys[pk->get_public_key()] = *pk;
                        account_name  nn;
                        std::stoul(names[i], &nn);
                        _my->_producers.insert(nn);
                        i++;
                    };


                }

//                _my->_production_enabled = options.at("enable-stale-production").as<bool>();
                _my->_production_enabled = true;
            }

            void producer_plugin::plugin_startup() {
                //for test add , remove later
                _my->_producers.insert(NEWS_SYSTEM_ACCOUNT_NAME);
                auto test_pk = fc::ecc::private_key::regenerate(fc::sha256::hash(std::string("pk")));
                auto test_public_key = public_key_type(test_pk.get_public_key());
                _my->_private_keys[test_public_key] = test_pk;




                if(_my->_production_enabled){
                    _my->schedule_production_loop();
                }
                else{
                    elog("No producer configured! Please add witness IDs and private keys to configuration.");
                }


            }

            void producer_plugin::plugin_shutdown() {

            }
        }   //namespace producer_plugin
    }//namespace plugins
}//namespace news