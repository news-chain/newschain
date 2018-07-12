//
// Created by boy on 18-6-12.
//





#include <news/plugins/chain_plugin/chain_plugin.hpp>
#include <fc/io/json.hpp>
#include <fc/thread/future.hpp>
#include <thread>

#include <boost/lockfree/queue.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/thread/future.hpp>
//#include <boost/prom>


namespace news {
    namespace plugins {
        namespace chain_plugin {
            namespace detail {


                struct generate_block_request {
                    generate_block_request(const fc::time_point_sec w, const news::base::account_name &wo,
                                           const fc::ecc::private_key &priv_key, uint32_t s) :
                            when(w),
                            witness_owner(wo),
                            block_signing_private_key(priv_key),
                            skip(s) {}

                    const fc::time_point_sec when;
                    const news::base::account_name &witness_owner;
                    const fc::ecc::private_key &block_signing_private_key;
                    uint32_t skip;
                    signed_block block;
                };

                typedef fc::static_variant<const signed_block *, const signed_transaction *, generate_block_request *> write_request_ptr;
                typedef fc::static_variant<boost::promise<void> *, fc::future<void> *> promise_ptr;

                struct write_context {
                    write_request_ptr req_ptr;
                    uint32_t skip = 0;
                    bool success = true;
                    fc::optional<fc::exception> except;
                    promise_ptr prom_ptr;
                };


                class chain_plugin_impl {
                public:
                    chain_plugin_impl() : write_queue(128) {}

                    ~chain_plugin_impl() {}


                    void start_write_processing();

                    void stop_write_processing();

                    uint64_t shared_memory_size = 0;
                    uint16_t shared_file_full_threshold = 0;
                    uint16_t shared_file_scale_rate = 0;
                    bool replay = false;
                    bool resync = false;
                    boost::filesystem::path shared_memory_path;
                    uint32_t flush_state_interval = 0;
                    uint32_t stop_replay_at = 0;

                    chain::database db;

                    bool running = true;
                    std::shared_ptr<std::thread> write_processor_thread;
                    boost::lockfree::queue<write_context *> write_queue;
                    uint16_t write_lock_hold_time = 500;  //ms
                };


                struct write_request_visitor {
                    write_request_visitor() {}

                    database *db;
                    uint32_t skip = 0;
                    fc::optional<fc::exception> *except;

                    typedef bool result_type;

                    bool operator()(const signed_block *block) {
                        bool result = false;

                        try {
//                                STATSD_START_TIMER( chain, write_time, push_block, 1.0f )
                            result = db->push_block(*block, skip);
//                                STATSD_STOP_TIMER( chain, write_time, push_block )
                        }
                        catch (fc::exception &e) {
                            *except = e;
                        }
                        catch (...) {
                            *except = fc::unhandled_exception(
                                    FC_LOG_MESSAGE(warn, "Unexpected exception while pushing block."),
                                    std::current_exception());
                        }

                        return result;
                    }

                    bool operator()(const signed_transaction *trx) {
                        bool result = false;

                        try {
//                                STATSD_START_TIMER( chain, write_time, push_tx, 1.0f )
                            db->push_transaction(*trx);
//                                STATSD_STOP_TIMER( chain, write_time, push_tx )

                            result = true;
                        }
                        catch (fc::exception &e) {
                            *except = e;
                        }
                        catch (...) {
                            *except = fc::unhandled_exception(
                                    FC_LOG_MESSAGE(warn, "Unexpected exception while pushing block."),
                                    std::current_exception());
                        }

                        return result;
                    }

                    bool operator()(generate_block_request *req) {
                        bool result = false;

                        try {
//                                STATSD_START_TIMER( chain, write_time, generate_block, 1.0f )
                            req->block = db->generate_block(
                                    req->when,
                                    req->witness_owner,
                                    req->block_signing_private_key,
                                    req->skip
                            );
//                                STATSD_STOP_TIMER( chain, write_time, generate_block )

                            result = true;
                        }
                        catch (fc::exception &e) {
                            *except = e;
                        }
                        catch (...) {
                            *except = fc::unhandled_exception(
                                    FC_LOG_MESSAGE(warn, "Unexpected exception while pushing block."),
                                    std::current_exception());
                        }

                        return result;
                    }
                };

                struct request_promise_visitor {
                    request_promise_visitor() {}

                    typedef void result_type;

                    template<typename T>
                    void operator()(T *t) {
                        t->set_value();
                    }
                };

                void chain_plugin_impl::start_write_processing() {
                    write_processor_thread = std::make_shared<std::thread>([&]() {
                        bool is_syncing = true;
                        write_context *cxt;
                        fc::time_point_sec start = fc::time_point::now();
                        write_request_visitor req_visitor;
                        req_visitor.db = &db;

                        request_promise_visitor promise_visitor;


                        /* This loop monitors the write request queue and performs writes to the database. These
                          * can be blocks or pending transactions. Because the caller needs to know the success of
                          * the write and any exceptions that are thrown, a write context is passed in the queue
                          * to the processing thread which it will use to store the results of the write. It is the
                          * caller's responsibility to ensure the pointer to the write context remains valid until
                          * the contained promise is complete.
                          *
                          * The loop has two modes, sync mode and live mode. In sync mode we want to process writes
                          * as quickly as possible with minimal overhead. The outer loop busy waits on the queue
                          * and the inner loop drains the queue as quickly as possible. We exit sync mode when the
                          * head block is within 1 minute of system time.
                          *
                          * Live mode needs to balance between processing pending writes and allowing readers access
                          * to the database. It will batch writes together as much as possible to minimize lock
                          * overhead but will willingly give up the write lock after 500ms. The thread then sleeps for
                          * 10ms. This allows time for readers to access the database as well as more writes to come
                          * in. When the node is live the rate at which writes come in is slower and busy waiting is
                          * not an optimal use of system resources when we could give CPU time to read threads.
                          */

                        while (running) {
                            if (!is_syncing) {
                                start = fc::time_point::now();
                            }


                            if (write_queue.pop(cxt)) {
                                db.with_write_lock([&]() {
                                    while (true) {
                                        req_visitor.skip = cxt->skip;
                                        req_visitor.except = &(cxt->except);
                                        cxt->success = cxt->req_ptr.visit(req_visitor);
                                        cxt->prom_ptr.visit(promise_visitor);
                                        if (is_syncing && start - db.head_block_time() < fc::minutes(1)) {
                                            start = fc::time_point::now();
                                            break;
                                        }
                                        if (!is_syncing && write_lock_hold_time >= 0 &&
                                            fc::time_point::now() - start > fc::milliseconds(write_lock_hold_time)) {
                                            break;
                                        }

                                        if (!write_queue.pop(cxt)) {
                                            break;
                                        }

                                    }//while
                                });//with_write_lock
                            }//

                            if (!is_syncing) {
                                boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
                            }

                        }//while
                    });
                }

                void chain_plugin_impl::stop_write_processing() {
                    running = false;
                    if (write_processor_thread) {
                        write_processor_thread->join();
                    }
                    write_processor_thread.reset();
                }

            }//namespace detail

            using detail::write_context;
            using detail::generate_block_request;

            chain_plugin::chain_plugin() : _my(new detail::chain_plugin_impl()) {

            }

            chain_plugin::~chain_plugin() {

            }

            void chain_plugin::set_program_options(options_description &cli, options_description &cfg) {

                cfg.add_options()
                        ("shared-file-size", bpo::value<std::string>()->default_value("1G"),
                         "size of shared memory size.")
                        // ("shared-file-dir", bpo::value<bfs::path>()->default_value("blockchain"), "the location of the chain shared memory files (absolute path or relative to application data dir)")
                        ("flush-state-interval", bpo::value<uint32_t>()->default_value(1000),
                         "flush shared memory changes to disk every N blocks")
                        ("shared-file-full-threshold", bpo::value<uint16_t>()->default_value(9000),
                         "A 2 precision percentage (0-10000) that defines the threshold for when to autoscale the shared memory file. Setting this to 0 disables autoscaling. Recommended value for consensus node is 9500 (95%). Full node is 9900 (99%)")
                        ("shared-file-scale-rate", bpo::value<uint16_t>()->default_value(2000),
                         "A 2 precision percentage (0-10000) that defines how quickly to scale the shared memory file. When autoscaling occurs the file's size will be increased by this percent. Setting this to 0 disables autoscaling. Recommended value is between 1000-2000 (10-20%)");


                cli.add_options()
                        ("replay-blockchain", bpo::bool_switch()->default_value(false),
                         "clear database and replay all blocks.")
                        ("resync-blockchain", bpo::bool_switch()->default_value(false),
                         "clear database and block log, sync from p2p network.")
                        ("stop-replay-at-block", bpo::value<uint32_t>(),
                         "Stop and exit after reaching given block number.");
            }

            void chain_plugin::plugin_initialize(const variables_map &options) {
                try {
                    _my->shared_memory_path = news::app::application::getInstance().get_data_path() / "blockchain";
                    _my->flush_state_interval = options.at("flush-state-interval").as<uint32_t>();
                    _my->shared_memory_size = fc::parse_size(options.at("shared-file-size").as<string>());
//                    _my->stop_replay_at = options.at("stop-replay-at-block").as<uint32_t>();
                    _my->stop_replay_at = 0;
                    _my->shared_file_scale_rate = options.at("shared-file-scale-rate").as<uint16_t>();
                    _my->shared_file_full_threshold = options.at("shared-file-full-threshold").as<uint16_t>();


                    _my->replay = options.at("replay-blockchain").as<bool>();
                    _my->resync = options.at("resync-blockchain").as<bool>();
                } catch (const fc::exception &e) {
                    elog("plugin init error ${e} , ${p}", ("e", e.what())("p", this->get_name()));
                }

            }

            void chain_plugin::plugin_startup() {





                    chain::open_db_args db_open_args;
                    db_open_args.data_dir = app::application::getInstance().get_data_path() / "blockchain";
                    db_open_args.shared_mem_dir = _my->shared_memory_path;
                    db_open_args.shared_mem_size = _my->shared_memory_size;
                    db_open_args.shared_file_full_threshold = _my->shared_file_full_threshold;
                    db_open_args.shared_file_scale_rate = _my->shared_file_scale_rate;
                    db_open_args.stop_replay_at = _my->stop_replay_at;

                    _my->start_write_processing();

                    if (_my->resync) {
                        //TODO sync
                        wlog("resync blockchain");
                        _my->db.wipe(app::application::getInstance().get_data_path(), db_open_args.shared_mem_dir,
                                     true);
                    }


                    if (_my->replay) {
                        //TODO stop at blocks num ?
                        _my->db.reindex(db_open_args);
                    } else {
                        ilog("open database dir: ${d}", ("d", _my->shared_memory_path.string()));
                        try {
                            _my->db.open(db_open_args);

                        } catch (const fc::exception &e) {
                            elog("open database error ${e}, try to replay blockchain", ("e", e.to_detail_string()));
                            _my->db.reindex(db_open_args);
                        }

                    }

//                on_sync();
            }

            void chain_plugin::plugin_shutdown() {
                ilog("close chain database.");
                _my->stop_write_processing();
                _my->db.close();
                ilog("database close successfully.");
            }

            news::chain::signed_block
            chain_plugin::generate_block(const fc::time_point_sec when, const news::base::account_name &producer,
                                         const fc::ecc::private_key &sign_pk, uint32_t skip) {
                generate_block_request req(when, producer, sign_pk, skip);
                boost::promise<void> prom;
                write_context cxt;
                cxt.req_ptr = &req;
                cxt.prom_ptr = &prom;

                _my->write_queue.push(&cxt);

                prom.get_future().get();
                if (cxt.except) {
                    throw *(cxt.except);
                }
                FC_ASSERT(cxt.success, "block generate error.");

                return req.block;
            }

            const database &chain_plugin::get_database() const {
                return _my->db;
            }

            database &chain_plugin::get_database() {
                return _my->db;
            }

            void chain_plugin::accept_transaction(const news::chain::signed_transaction &trx) {
//                ilog("accept_transaction ${trx}", ("trx", trx));
                boost::promise<void> prom;
                write_context cxt;
                cxt.req_ptr = &trx;
                cxt.prom_ptr = &prom;
                _my->write_queue.push(&cxt);

                prom.get_future().get();
                if (cxt.except) {
                    throw *(cxt.except);
                }

            }

            void chain_plugin::accept_block(const news::chain::signed_block &block, bool syncing, uint32_t skip) {

                elog("accept_block #${b}, size ${s} time ${t}",
                     ("b", block.block_num())("s", block.transactions.size())("t", block.timestamp));
                if (syncing) {

                }
                boost::promise<void> prom;
                write_context cxt;
                cxt.req_ptr = &block;
                cxt.skip = skip;

                cxt.prom_ptr = &prom;
                _my->write_queue.push(&cxt);

                prom.get_future().get();
                if (cxt.except) {
                    throw *(cxt.except);
                }
            }


        }//namespace chain_plugin
    }//namespace plugins
}//namespace news