//
// Created by boy on 18-6-12.
//



#include <news/plugins/producer_plugin/producer_plugin.hpp>
#include <news/protocol/key_conversion.hpp>



namespace news{
    namespace plugins{
        namespace producer_plugin{


            using namespace news::protocol;

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

                    std::map<news::protocol::public_key_type, fc::ecc::private_key>     _private_keys;
                    news::plugins::chain_plugin::chain_plugin&                           _chain_plugin;
                    news::chain::database&                                              _db;
                    std::set< news::protocol::account_name >                            _producers;

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
                    fc::time_point now = fc::time_point::now();
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
                    public_key_type key = public_key_type();
                    auto private_key = _private_keys.find(key);
                    if(private_key == _private_keys.end()){
                        cap("not find produces private key", producer);
                        return block_production_condition::no_private_key;
                    }

                    //TODO required_witness_participation ?

                    if(llabs((scheduled_time - now).count()) > fc::microseconds(500).count() ){
                        cap("scheduled_time", scheduled_time)("now", now);
                        return block_production_condition::lag;
                    }

                    //TODO add skip flag
                    auto block = _chain_plugin.generate_block(now, producer, private_key->second, 0);



                    return block_production_condition::produced;

                }

                block_production_condition::block_production_condition_enum producer_plugin_impl::block_production_loop() {
                    if(fc::time_point::now() < fc::time_point(NEWS_GENESIS_TIME)){
                        wlog("waiting until genesis to produce block : ${t}", ("t", NEWS_GENESIS_TIME));
                        return block_production_condition::wait_for_genesis;
                    }
                    block_production_condition::block_production_condition_enum  result;
                    fc::mutable_variant_object capture;
                    try {
                        result = maybe_produce_block(capture);
                    }
//                    catch (const fc::exception &e){
////                        elog("generate block ${e}", ("e", e.to_detail_string() ));
//
//                    }
                    catch (...){
                        return block_production_condition::exception_producing_block;
                    }

                    return result;
                }


            }//namespace detail


            producer_plugin::producer_plugin():_my( new detail::producer_plugin_impl( news::app::application::getInstance().get_io_service() ) ) {

            }


            producer_plugin::~producer_plugin() {

            }

            void producer_plugin::set_program_options(options_description &, options_description &cfg) {
                cfg.add_options()
                        ("enable-stale-production", bpo::bool_switch()->default_value(false), "Enable block production, even if the chain is stale.")
                        ("producer-name", bpo::value<std::vector<std::string>>()->composing(), "producer name vector")
                        ("private-key", bpo::value<std::vector<std::string>>()->composing(), "private keys for producer name");


            }

            void producer_plugin::plugin_initialize(const variables_map &options) {
                if(options.count("private-key")){
                    const std::vector<std::string> keys = options["private-key"].as<std::vector<std::string>>();
                    for(const std::string &wif_key : keys){
                        fc::optional<fc::ecc::private_key> pk = news::protocol::wif_to_key(wif_key);
                        FC_ASSERT(pk.valid(), "unable to parse private key");
                        _my->_private_keys[pk->get_public_key()] = *pk;
                    }
                }

//                _my->_production_enabled = options.at("enable-stale-production").as<bool>();
                _my->_production_enabled = true;
            }

            void producer_plugin::plugin_startup() {
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