//
// Created by boy on 18-6-12.
//



#include <news/plugins/producer_plugin/producer_plugin.hpp>


namespace news{
    namespace plugins{
        namespace producer_plugin{

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
                    producer_plugin_impl(boost::asio::io_service &io):_timer(io){}
                    ~producer_plugin_impl(){}

                    void schedule_production_loop();
                    block_production_condition::block_production_condition_enum  block_production_loop();
                    block_production_condition::block_production_condition_enum  maybe_produce_block(fc::mutable_variant_object &cap);

                    boost::program_options::variables_map   _options;
                    bool                                    _production_enabled;
                    uint32_t                                _required_producer_participation = 0;
                    uint32_t                                _production_skip_flags;
//                    std::map<news::chain>

                    boost::asio::deadline_timer             _timer;
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
                    fc::time_point_sec now_next = now + fc::microseconds(500000);
                    if(!_production_enabled){
                        
                    }


                }

                block_production_condition::block_production_condition_enum producer_plugin_impl::block_production_loop() {
                    if(fc::time_point::now() < fc::time_point(NEWS_GENESIS_TIME)){
                        wlog("waiting until genesis to produce block : ${t}", ("t", NEWS_GENESIS_TIME));
                        return block_production_condition::wait_for_genesis;
                    }
                    block_production_condition::block_production_condition_enum  result;
                    fc::mutable_variant_object capture;
                    try {
                        maybe_produce_block(capture);
                    }
                    catch (const fc::exception &e){
//                        elog("generate block ${e}", ("e", e.to_detail_string() ));
                        return block_production_condition::exception_producing_block;
                    }
                    catch (...){

                    }


                }


            }

















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

            }

            void producer_plugin::plugin_startup() {

            }

            void producer_plugin::plugin_shutdown() {

            }
        }   //namespace producer_plugin
    }//namespace plugins
}//namespace news