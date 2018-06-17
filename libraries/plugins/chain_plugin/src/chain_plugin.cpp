//
// Created by boy on 18-6-12.
//





#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <fc/io/json.hpp>


namespace news{
    namespace plugins{
        namespace chain_plugin{
            namespace detail{

                class chain_plugin_impl{
                public:
                    chain_plugin_impl(){}
                    ~chain_plugin_impl(){}


                    void start_write_processing();
                    void stop_write_processing();
                    uint64_t                        shared_memory_size = 0;
                    uint64_t                        shared_file_full_threshold = 0;
                    uint16_t                        shared_file_scale_rate = 0;
                    bool                            replay = false;
                    bool                            resync = false;
                    boost::filesystem::path         shared_memory_path;
                    uint32_t                        flush_state_interval = 0;
                    uint32_t                        stop_replay_at = 0;

                    chain::database                 db;

                };

                void chain_plugin_impl::start_write_processing() {

                }

                void chain_plugin_impl::stop_write_processing() {

                }

            }


            chain_plugin::chain_plugin():_my(new detail::chain_plugin_impl()) {

            }

            chain_plugin::~chain_plugin() {

            }

            void chain_plugin::set_program_options(options_description &cli, options_description &cfg) {

                cfg.add_options()
                        ("shared-file-size", bpo::value<std::string>()->default_value("20G"), "size of shared memory size.")
                       // ("shared-file-dir", bpo::value<bfs::path>()->default_value("blockchain"), "the location of the chain shared memory files (absolute path or relative to application data dir)")
                        ("flush-state-interval", bpo::value<uint32_t >()->default_value(1000), "flush shared memory changes to disk every N blocks");


                cli.add_options()
                        ("replay-blockchain", bpo::bool_switch()->default_value(false), "clear database and replay all blocks.")
                        ("resync-blockchain", bpo::bool_switch()->default_value(false), "clear database and block log, sync from p2p network.")
                        ("stop-replay-at-block", bpo::value<uint32_t>(), "Stop and exit after reaching given block number.");
            }

            void chain_plugin::plugin_initialize(const variables_map &options) {
                try {

                    _my->shared_memory_path = news::app::application::getInstance().get_data_path() / "blockchain";

                    _my->flush_state_interval = options.at("flush-state-interval").as<uint32_t>();
                    _my->shared_memory_size = fc::parse_size( options.at("shared-file-size").as<string>());
//                    _my->stop_replay_at = options.at("stop-replay-at-block").as<uint32_t>();
                    _my->stop_replay_at = 0;

                    _my->replay = options.at("replay-blockchain").as<bool>();
                }catch (const fc::exception &e){
                    elog("plugin init error ${e} , ${p}", ("e", e.what())("p", this->get_name()));
                }

            }

            void chain_plugin::plugin_startup() {


                if(_my->resync){
                    //TODO sync
                }

                chain::open_db_args db_open_args;
                db_open_args.data_dir = app::application::getInstance().get_data_path() / "blockchain";
                db_open_args.shared_mem_dir = _my->shared_memory_path;
                db_open_args.shared_mem_size = _my->shared_memory_size;
                db_open_args.shared_file_full_threshold = _my->shared_file_full_threshold;
                db_open_args.shared_file_scale_rate = _my->shared_file_scale_rate;
                db_open_args.stop_replay_at = _my->stop_replay_at;


                if(_my->replay){
                    //TODO stop at blocks num ?
                    _my->db.reindex(db_open_args);
                }
                else{
                    ilog("open database dir: ${d}", ("d", _my->shared_memory_path.string()));
                    try {
                        _my->db.open(db_open_args);

                    }catch (const fc::exception &e){
                        elog("open database error ${e}, try to replay blockchain", ("e", e.what()));
                        //TODO replay blcokchain
                    }

                }
            }

            void chain_plugin::plugin_shutdown() {

            }

            news::chain::signed_block
            chain_plugin::generate_block(const fc::time_point_sec when, const news::base::account_name &producer,
                                         const fc::ecc::private_key &sign_pk, uint32_t skip) {
                return  _my->db.generate_block(when, producer, sign_pk, (validation_steps)skip);
            }

            const database &chain_plugin::get_database() const {
                return _my->db;
            }

            database &chain_plugin::get_database() {
                return _my->db;
            }


        }//namespace chain_plugin
    }//namespace plugins
}//namespace news