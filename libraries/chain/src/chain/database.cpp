//
// Created by boy on 18-6-11.
//

#include <news/chain/database.hpp>
#include <fc/scoped_exit.hpp>
#include <app/application.hpp>

namespace news {
    namespace chain {

        namespace detail {
        }//news::chain::detail


        class database_impl {
        public:
            database_impl(database &self) : _self(self), _eveluator_registry(self) {}

            database &_self;
            evaluator_registry<news::base::operation> _eveluator_registry;
        };


        database::database() : _my(new database_impl(*this)) {

        }

        database::~database() {
            clear_pending();
        }

        void database::open(const news::chain::open_db_args &args) {
            chainbase::database::open(args.shared_mem_dir, args.chainbase_flag, args.shared_mem_size);

            _block_log.open(args.data_dir / "block_log");

            initialize_indexes();
            init_genesis(args);
            register_evaluator();


            auto log_header = _block_log.head();
            with_write_lock([&]() {
                undo_all();
                FC_ASSERT(revision() == head_block_num(), "Chainbase revision does not match head block num",
                          ("rev", revision())("head_block ", head_block_num()));
                //TODO do_validate_invariants

            });


            if (head_block_num()) {
                auto head_block = _block_log.read_block_by_num(head_block_num());
                FC_ASSERT(head_block.valid() && head_block_id() == head_block->id(),
                          "Chain state does not match block log. Please reindex blockchain.");
                _fork_database.start_block(*head_block);
            }

            _shared_file_full_threshold = args.shared_file_full_threshold;
            _shared_file_scale_rate = args.shared_file_scale_rate;

            //TODO init hardforks

            with_write_lock([&]() {
                init_hardforks();
            });
        }

        void database::initialize_indexes() {
            add_index<dynamic_global_property_object_index>();
            add_index<block_summary_index>();
            add_index<transaction_obj_index>();
            add_index<news::base::account_object_index>();
            add_index<operation_obj_index>();
            add_index<account_history_obj_index>();
            add_index<account_authority_index>();
            add_index<hardfork_property_index>();
        }

        uint32_t database::get_slot_at_time(fc::time_point_sec when) {
            fc::time_point_sec first_slot_time = get_slot_time(1);
            if (when < first_slot_time) {
                return 0;
            }
            return (when - first_slot_time).to_seconds() / NEWS_BLOCK_INTERVAL + 1;
        }

        fc::time_point_sec database::get_slot_time(uint32_t slot_num) const {
            if (slot_num == 0) {
                return fc::time_point_sec();
            }

            const dynamic_global_property_object &gpo = get_global_property_object();
            if (gpo.head_block_num == 0) {
                fc::time_point_sec gensis_time = gpo.time;
                return (gensis_time + slot_num * NEWS_BLOCK_INTERVAL);
            }

            int64_t head_block_abs_slot = gpo.time.sec_since_epoch() / NEWS_BLOCK_INTERVAL;
            fc::time_point_sec head_slot_time(head_block_abs_slot * NEWS_BLOCK_INTERVAL);
            return head_slot_time + (slot_num * NEWS_BLOCK_INTERVAL);

        }


        const dynamic_global_property_object &database::get_global_property_object() const {
            try {
                return get<dynamic_global_property_object>();
            } FC_CAPTURE_AND_RETHROW()
        }

        uint32_t database::head_block_num() const {
            return get_global_property_object().head_block_num;
        }


        fc::time_point_sec database::head_block_time() const {
            return get_global_property_object().time;
        }


        block_id_type database::head_block_id() const {
            return get_global_property_object().head_block_id;
        }


        account_name database::get_scheduled_producer(uint32_t num) const {
            const auto &gpo = get_global_property_object();
            static vector<account_name> producers = {NEWS_SYSTEM_ACCOUNT_NAME, NEWS_SYSTEM_ACCOUNT_NAME+1};
            uint64_t current_slot = gpo.current_aslot + num;
//            elog("get_scheduled_producer ${n}", ("n", current_slot));
            return producers[current_slot % producers.size()];
//            return producers[(gpo.head_block_num ) % producers.size()];
        }

        signed_block database::generate_block(const fc::time_point_sec when, const account_name &producer,
                                              const fc::ecc::private_key private_key_by_signed, uint64_t skip) {
            auto start = fc::time_point::now();
            signed_block result;
            with_skip_flags(skip, [&]() {
                try {
                    result = _generate_block(when, producer, private_key_by_signed);
                } FC_CAPTURE_AND_RETHROW((result))
            });
            ilog("-----skip generate_block time ${t}", ("t", (fc::time_point::now() - start).count()));

            return result;
        }

        signed_block database::_generate_block(const fc::time_point_sec when, const account_name &producer,
                                               const fc::ecc::private_key private_key_by_signed) {
            //
            auto start = fc::time_point::now();
            signed_block pengding_block;

            _pending_trx_session.reset();
            _pending_trx_session = start_undo_session();


            //TODO block_header_size
            size_t total_block_size = fc::raw::pack_size(signed_block_header()) + 4;
            uint64_t postponed_tx_count = 0;
            uint32_t count = 10000;
            ilog("_pending_trx.size:${s}", ("s", _pending_trx.size()));

            for (const signed_transaction &tx : _pending_trx) {
                if (tx.expiration < when) {
                    continue;
                }
                if (count == 0) {
                    break;
                }
                uint64_t trx_size = fc::raw::pack_size(tx);
                uint64_t new_total_size = total_block_size + trx_size;
                if (new_total_size >= NEWS_MAX_BLOCK_SIZE) {
                    postponed_tx_count++;
//                    continue;
                    break;
                }

                try {
                    auto temp_seesion = start_undo_session();
                    _apply_transaction(tx);
                    temp_seesion.squash();


                    total_block_size += trx_size;
                    pengding_block.transactions.push_back(tx);
                    count--;
                } catch (const fc::exception &e) {
                    elog("generate_block : ${e}  trx:${t}", ("e", e.to_detail_string())("t", tx));
                } catch (...) {
                    elog("generate_block : ${e}  trx:${t}", ("t", tx));
                }
            }
            if (postponed_tx_count > 0) {
                elog("Postponed ${n} transactions due to block size limit", ("n", postponed_tx_count));
            }

            _pending_trx_session.reset();


            pengding_block.timestamp = when;
            pengding_block.previous = head_block_id();
            pengding_block.transaction_merkle_root = pengding_block.caculate_merkle_root();
            pengding_block.producer = producer;




            if (!(_skip_flags & skip_producer_signature)) {
                pengding_block.sign(private_key_by_signed);
            }

            if (!(_skip_flags & skip_block_size_check)) {
                FC_ASSERT(fc::raw::pack_size(pengding_block) <= NEWS_MAX_BLOCK_SIZE);
            }



            push_block(pengding_block, _skip_flags);
            ilog("generate_block time ${t}", ("t", (fc::time_point::now() - start).count()));
            return pengding_block;
        }

        void database::init_genesis(open_db_args args) {

            if (!find<dynamic_global_property_object>()) {
                with_write_lock([&]() {
                    create<dynamic_global_property_object>([](dynamic_global_property_object &obj) {
                        obj.time = NEWS_GENESIS_TIME;
                    });

                    for (int i = 0; i < 3; i++) {
                        create<account_object>([&](account_object &obj) {
                            obj.name = NEWS_SYSTEM_ACCOUNT_NAME + i;
//                            obj.balance.amount = 1000000000000L;
                            obj.balance.amount = INT64_MAX;
//                            to_shared_string(NEWS_INIT_PUBLIC_KEY, obj.public_key);
                        });
                    }


                    for(int i = 0;i < 3; i++){
                        create<account_authority_object>([&](account_authority_object &obj){
                            obj.name = NEWS_SYSTEM_ACCOUNT_NAME + i;
                            obj.posting = {news::base::public_key_type(NEWS_INIT_PRIVATE_KEY.get_public_key()), 1};
                            obj.owner = {news::base::public_key_type(NEWS_INIT_PRIVATE_KEY.get_public_key()), 1};
                        });
                    }


                    for (uint32_t i = 0; i < 0x10000; i++) {
                        create<block_summary_object>([](block_summary_object &) {
                        });
                    }


                    create<account_object>([&](account_object &obj){
                        obj.name = NEWS_SYSTEM_ACCEPT_NAME;
//                        to_shared_string(NEWS_ACCEPT_NAME_PUBLIC_KEY, obj.public_key);
                    });

                    create<account_authority_object>([&](account_authority_object &obj){
                        obj.name = NEWS_SYSTEM_ACCEPT_NAME;
//                        to_shared_string(NEWS_ACCEPT_NAME_PUBLIC_KEY, obj.public_key);
                        obj.posting = {news::base::public_key_type(NEWS_ACCEPT_NAME_PRIVATE_KEY.get_public_key()), 1};
                        obj.owner = {news::base::public_key_type(NEWS_ACCEPT_NAME_PRIVATE_KEY.get_public_key()), 1};
                    });


                    create<hardfork_property_object>([](hardfork_property_object &obj){
                        obj.processed_hadrdfork.push_back(NEWS_GENESIS_TIME);
                    });




                });


            }

        }

        void database::update_global_property_object(const signed_block &block) {
            const auto &gpo = get_global_property_object();

            uint32_t miss_blocks = get_slot_at_time(block.timestamp);
            assert(miss_blocks != 0);


            modify(gpo, [block, miss_blocks](dynamic_global_property_object &obj) {
                obj.time = block.timestamp;
                obj.head_block_num = block.block_num();
                obj.head_block_id = block.id();
                obj.current_aslot += miss_blocks;
            });





        }

        void database::update_last_irreversible_block() {
            static_assert(IRREVERSIBLE_BLOCK_NUM > 0, "irreversible_block_num must be nonzero.");
            //update
            const auto &gpo = get_global_property_object();
            if (gpo.head_block_num >= IRREVERSIBLE_BLOCK_NUM) {
                modify(gpo, [&](dynamic_global_property_object &obj) {
                    obj.last_irreversible_block_num = head_block_num() - IRREVERSIBLE_BLOCK_NUM;
                });
            }


            commit(gpo.last_irreversible_block_num);

            if (!(_skip_flags & skip_block_log)) {
                uint32_t log_head_num = 0;
                const auto &temp_head = _block_log.head();
                if (temp_head) {
                    log_head_num = temp_head->block_num();
                }
                if (log_head_num < gpo.last_irreversible_block_num) {
                    while (log_head_num < gpo.last_irreversible_block_num) {
                        shared_ptr<fork_item> fitem = _fork_database.fetch_block_on_main_branch_by_number(
                                log_head_num + 1);
                        FC_ASSERT(fitem,
                                  "Current fork in the fork database does not contain the last_irreversible_block");
                        _block_log.append(fitem->data);
//                        elog("block log append block ${b}", ("b", fitem->data.block_num()));
                        log_head_num++;
                        _block_log.flush();
                    }
                }
            }

            _fork_database.set_max_size(gpo.head_block_num - gpo.last_irreversible_block_num + 1);
        }


        void database::apply_block(const signed_block &block, uint64_t skip) {
            try {
                auto block_num = block.block_num();
                //TODO checkpoints

                with_skip_flags(skip, [&]() {
                    _apply_block(block, skip);
                });


                if (_flush_blocks != 0) {
                    if (_next_flush_block == 0) {
                        uint32_t lep = block_num + 1 + _flush_blocks * 9 / 10;
                        uint32_t rep = block_num + 1 + _flush_blocks;

                        uint32_t span = rep - lep;
                        uint32_t nn = lep;
                        if (span > 0) {
                            uint64_t now = uint64_t(fc::time_point::now().time_since_epoch().count());
                            nn += now % span;
                        }
                        _next_flush_block = nn;
                    }

                    if (_next_flush_block == block_num) {
                        _next_flush_block = 0;

                        chainbase::database::flush();
                        elog("flush database shared memory at block ${b}", ("b", block_num));
                    }
                }

            } FC_CAPTURE_AND_RETHROW()
        }

        void database::_apply_block(const signed_block &block, uint64_t skip) {
            try {
                _current_block_num = block.block_num();

                if(BOOST_UNLIKELY(block.block_num() == 1)){
                    uint32_t n = 0;
                    for(; n < NEWS_HARDFORK_NUM ; n++){
                        if(_hardfork_times[n + 1] > block.timestamp){
                            break;
                        }
                    }

                    if(n > 0){
                        ilog("processing ${n} genesis hardfork", ("n", n));
                        set_hardfork(n, true);
                    }

                }

                if (!(skip & skip_merkle_check)) {
                    auto merkle_root = block.caculate_merkle_root();
                    try {
                        FC_ASSERT(merkle_root == block.transaction_merkle_root, "merkle check failed",
                                  ("new_block merkle root", block.producer_signature)("caculate merkle root ",
                                                                                      merkle_root));
                    } catch (fc::assert_exception &e) {
                        // TODO?
                        elog("_apply_block ", ("e", e.what()));
                        throw e;
                    }
                }


                auto block_size = fc::raw::pack_size(block);
                if (block_size < NEWS_MIN_BLOCK_SIZE) {
                    elog("block size si too small ${block_num} ${block_size}",
                         ("block_num", block.block_num())("block_size", block_size));
//                   elog("block : ${b}", ("b", block.timestamp));
                }
                auto start = fc::time_point::now();
                for (const auto &trx : block.transactions) {
                    apply_transaction(trx, skip);
                }
                if (block.transactions.size() > 3000) {
                    elog("apply_operation time : ${t}ms size ${s}",
                         ("t", (fc::time_point::now().time_since_epoch() - start.time_since_epoch()).count() / 1000)(
                                 "s", block.transactions.size()));
                }

                update_global_property_object(block);
                update_last_irreversible_block();
                create_block_summary(block);
                clear_expired_transactions();
                process_hardforks();
            } FC_CAPTURE_AND_RETHROW()
        }


        bool database::push_block(const signed_block &block, uint64_t skip) {
            bool result = false;
            with_skip_flags(skip, [&]() {
                without_pengding_transactions([&]() {
                    try {
                        result = _push_block(block, skip);
                    } FC_CAPTURE_AND_RETHROW((block))

                    check_free_memory(false, block.block_num());
                });


            });
            return result;
        }

        bool database::_push_block(const signed_block &block, uint64_t skip) {
//            elog("_push block  ${b}", ("b", block));
            try {
                if (!(skip & skip_fork_db)) {
                    shared_ptr<fork_item> new_block = _fork_database.push_block(block);
                    //TODO find producer?

                    //If the head block from the longest chain does not build off of the current head, we need to switch forks.
                    if (new_block->data.previous != head_block_id()) {

                        //If the newly pushed block is the same height as head, we get head back in new_head
                        //Only switch forks if new_head is actually higher than head
                        if (new_block->data.block_num() > head_block_num()) {


                            wlog("switching to for : ${id}", ("id", new_block->data.id()));
                            auto branches = _fork_database.fetch_branch_from(new_block->data.id(), head_block_id());

                            while (head_block_id() != branches.second.back()->data.previous){
                                elog("pop block head_block_id =${h} , previous ${p}", ("h", head_block_id())("p", branches.second.back()->data.previous));
                                pop_block();
                            }

//                        push all blocks on the new block
                            for (auto ritr = branches.first.rbegin(); ritr != branches.first.rend(); ritr++) {
                                ilog("push blocks from for ${n} ${id}", ("n", (*ritr)->data.block_num())("id", (*ritr)->data.id()));

                                fc::optional<fc::exception> exception;
                                try {
                                    auto session = start_undo_session();
                                    apply_block((*ritr)->data, skip);
                                    session.push();
                                } catch (const fc::exception &e) {
                                    exception = e;
                                }

                                if (exception) {
                                    wlog("exception thrown while switching forks ${e}",
                                         ("e", exception->to_detail_string()));
                                    // remove the rest of branches.first from the fork_db, those blocks are invalid
                                    while (ritr != branches.first.rend()) {
                                        _fork_database.remove((*ritr)->data.id());
                                        ritr++;
                                    }
                                    _fork_database.set_head(branches.second.front());

                                    // pop all blocks from the bad fork
                                    while (head_block_id() != branches.second.back()->data.previous) {
                                        pop_block();
                                    }

                                    for (auto itr = branches.second.rbegin(); itr != branches.second.rend(); itr++) {
                                        auto session = start_undo_session();
                                        apply_block((*itr)->data, skip);
                                        session.push();
                                    }
                                    throw *exception;
                                }

                            }
                            return true;
                        } else {
                            return false;
                        }
                    }
                }

                try {
                    auto session = start_undo_session();
                    apply_block(block, skip);
                    session.push();
                    return true;
                } catch (const fc::exception &e) {
                    elog("Failed to push new block:\n${e}", ("e", e.to_detail_string()));
                    _fork_database.remove(block.id());
                    throw;
                }
            } FC_CAPTURE_AND_RETHROW()

        }


        void database::create_block_summary(const signed_block &b) {
            try {
                oid<block_summary_object> sid(b.block_num() & 0xffff);
                modify(get<block_summary_object>(sid), [&](block_summary_object &obj) {
                    obj.block_id = b.id();
                });
            } FC_CAPTURE_AND_RETHROW((b))
        }

        bool database::is_know_transaction(const transaction_id_type &trx_id) const {
            try {
                const auto &trx_itr = get_index<transaction_obj_index>().indices().get<by_trx_id>();
                return trx_itr.find(trx_id) != trx_itr.end();
            } FC_CAPTURE_AND_RETHROW((trx_id))

        }

        void database::push_transaction(const signed_transaction &trx, uint64_t skip) {
            try {
                try {
//                    FC_ASSERT(fc::raw::pack(trx) <= NEWS_MAX_BLOCK_SIZE - 256);
                    set_producing(true);

                    with_skip_flags(skip, [&]() {
                        _push_transaction(trx);
                    });

                    set_producing(false);
                }
                catch (const fc::exception &e) {
                    set_producing(false);
                    throw e;
                }
                catch (...) {
                    set_producing(false);
                    throw;
                }

            } FC_CAPTURE_AND_RETHROW((trx))
        }

        void database::_push_transaction(const signed_transaction &trx) {
            if (!_pending_trx_session.valid()) {
                _pending_trx_session = start_undo_session();
            }
            auto temp_session = start_undo_session();
            _apply_transaction(trx);
            _pending_trx.push_back(trx);
            temp_session.squash();  //TODO how to work

        }

        void database::pop_block() {
            try {
                _pending_trx_session.reset();
                auto head_id = head_block_id();
                fc::optional<signed_block> head_block = fetch_block_by_id(head_id);
                FC_ASSERT(head_block.valid(), "there is no block to pop");
                _fork_database.pop_block();
                undo();

                //TODO record poped transaction , insert next blocks?
                _popped_tx.insert(_popped_tx.begin(), head_block->transactions.begin(), head_block->transactions.end());

            } FC_CAPTURE_AND_RETHROW()

        }

        fc::optional<signed_block> database::fetch_block_by_id(const block_id_type &id) const {
            try {
                auto b = _fork_database.fetch_block(id);
                if (!b) {
                    auto temp = _block_log.read_block_by_num(news::chain::block_header::num_from_id(id));
                    if (temp && temp->id() == id) {
                        return temp;
                    }
                    temp.reset();
                    return temp;
                }
                return b->data;
            } FC_CAPTURE_AND_RETHROW()
        }

        uint32_t database::reindex(const open_db_args &args) {
            try {
                ilog("reindex blockchain");
                wipe(args.data_dir, args.shared_mem_dir);
                open(args);
                _fork_database.reset();

                auto start = fc::time_point::now();

                uint64_t skip_flags =// skip_nothing;
                        skip_producer_signature |
                        skip_transaction_signatures |
                        skip_tapos_check |
                        skip_merkle_check |
                        skip_producer_schedule_check |
                        skip_authority_check |
                        skip_validate |
                        skip_validate_invariants |
                        skip_block_log;


                with_write_lock([&]() {
                    _block_log.set_locking(false);
                    auto itr = _block_log.read_block(0);
                    auto last_block_num = _block_log.read_head().block_num();
                    if (args.stop_replay_at > 0 && args.stop_replay_at < last_block_num) {
                        last_block_num = args.stop_replay_at;
                    }
                    ilog("current last_block_num ${b}", ("b", last_block_num));
                    //TODO stop at block num?

                    while (itr.first.block_num() != last_block_num) {
                        auto current_block_num = itr.first.block_num();
                        if (current_block_num % 10000 == 0) {
                            std::cerr << "now reindex block num :" << itr.first.block_num() << " spend: " << (fc::time_point::now() - start).count() / 1000000.0 << std::endl;
                        }

                        apply_block(itr.first, skip_flags);
                        check_free_memory(false, current_block_num);
                        itr = _block_log.read_block(itr.second);
                    }


                    apply_block(itr.first, skip_flags);
                    set_revision(head_block_num());

                    _block_log.set_locking(true);

                });

                if (_block_log.head()->block_num()) {
                    _fork_database.start_block(*_block_log.head());
                }


                auto end = fc::time_point::now();
                elog("Done reindexing blcoks, elapsed time ${t} sec",
                     ("t", (double) (end - start).count() / 1000000.0));

                //TODO return  stop at block num
                return _block_log.read_head().block_num();
            } FC_CAPTURE_AND_RETHROW()

        }

        void database::apply_transaction(const signed_transaction &trx, uint64_t skip) {
            with_skip_flags(skip, [&]() {
                _apply_transaction(trx);
            });

        }

        void database::_apply_transaction(const signed_transaction &trx) {


            if (!(_skip_flags & skip_validate)) {
                trx.validate();
            }
            _current_trx_id = trx.id();


            auto &trx_index = get_index<transaction_obj_index>().indices().get<by_trx_id>();

            transaction_id_type trx_id = trx.id();
            FC_ASSERT((_skip_flags | skip_transaction_dupe_check) || trx_index.find(trx_id) != trx_index.end(),
                      "Duplicate transaction check failed", ("trx id ", trx_id));

            if (BOOST_LIKELY(head_block_num()) > 0) {

                if (!(_skip_flags & skip_tapos_check)) {
                    const auto &tapos_block_summary = get<block_summary_object>(trx.ref_block_num);
                    FC_ASSERT(trx.ref_block_prefix == tapos_block_summary.block_id._hash[1],
                              "transaction tapos exception trx.ref_block_prefix ${t}, tapos_block_summary ${a}",
                              ("t", trx.ref_block_prefix)("a", tapos_block_summary.block_id._hash[1]));
                }
                fc::time_point_sec now = head_block_time();
                FC_ASSERT(trx.expiration >= now, "transacion expiration ${trx} now:${now}",
                          ("trx", trx)("now", now));

            }

            if (!(_skip_flags & (skip_transaction_signatures | skip_authority_check))) {
                get_key_by_name get_posting = [&](const account_name &name)  {
                    const auto &u_itr = get_index<account_authority_index>().indices().get<by_name>();
                    auto account_itr = u_itr.find(name);
                    FC_ASSERT(account_itr != u_itr.end(), "in verify_authority, cant find account ${a}", ("a", name));
//                    public_key_type pub_key(account_itr->posting.key);
                    return account_itr->posting.key;
                };

                get_key_by_name get_owner = [&](const account_name &name)  {
                    const auto &u_itr = get_index<account_authority_index>().indices().get<by_name>();
                    auto account_itr = u_itr.find(name);
                    FC_ASSERT(account_itr != u_itr.end(), "in verify_authority, cant find account ${a}", ("a", name));
//                    public_key_type pub_key(account_itr->owner.key);
                    return account_itr->owner.key;
                };

                try {
                    //TODO verity_authority
                    trx.verify_authority(get_posting, get_owner, get_chain_id());
                } catch (const fc::exception &e) {
                    //TOO catch exception
                    throw e;
                }

            }

            if (!(_skip_flags & skip_transaction_dupe_check)) {
                try{
                    create<transaction_object>([&](transaction_object &obj) {
                        obj.trx_id = trx_id;
                        obj.expiration = trx.expiration;
                        fc::raw::pack_to_buffer(obj.packed_trx, trx);
                    });
                }FC_CAPTURE_AND_RETHROW((trx))
            }

            _current_op_in_trx = 0;
            for (const auto &op : trx.operations) {
                try {
                    apply_operation(op);
                    _current_op_in_trx++;
                } FC_CAPTURE_AND_RETHROW((op));
            }

        }

        void database::apply_operation(const operation &op) {
            //TODO notification
            operation_notification note(op);

            _my->_eveluator_registry.get_evaluator(op).apply(op);

            note.block = _current_block_num;
            note.trx_in_block = _current_op_in_trx;
            note.trx_id = _current_trx_id;

            notify_post_apply_operation(note);
        }

        void database::register_evaluator() {
            _my->_eveluator_registry.register_evaluator<create_account_evaluator>();
            _my->_eveluator_registry.register_evaluator<transfer_evaluator>();
            _my->_eveluator_registry.register_evaluator<transfers_evaluator>();
        }

        fc::optional<signed_block> database::fetch_block_by_number(uint32_t block_num) {
            try {
                fc::optional<signed_block> b;
                auto result = _fork_database.fetch_block_by_number(block_num);
                if (result.size() == 1) {
                    b = result[0]->data;
                } else {
                    b = _block_log.read_block_by_num(block_num);
                }
                return b;
            } FC_CAPTURE_AND_RETHROW()
        }

        const chain_id_type &database::get_chain_id() {
            static chain_id_type chain_id = NEWS_CHAIN_ID;
            return chain_id;
        }

        void database::clear_pending() {
            _pending_trx.clear();

        }

        void database::clear_expired_transactions() {
            auto &trx_idx = get_index<transaction_obj_index>();
            const auto &deque_index = trx_idx.indices().get<by_expiration>();
            while ((!deque_index.empty()) && (head_block_time() > deque_index.begin()->expiration)) {
                remove(*deque_index.begin());
            }
        }


        bool database::is_know_block(const news::base::block_id_type &id) const {
            try {
                return fetch_block_by_id(id).valid();
            } FC_CAPTURE_AND_RETHROW((id))
        }

        uint32_t database::last_non_undoable_block_num() const {
            return get_global_property_object().last_irreversible_block_num;
        }

        const signed_transaction database::get_recent_transaction(const transaction_id_type &trx_id) const {
            try {
                auto &index = get_index<transaction_obj_index>().indices().get<by_trx_id>();
                auto itr = index.find(trx_id);
                FC_ASSERT(itr != index.end());
                signed_transaction trx;
                fc::raw::unpack_from_buffer(itr->packed_trx, trx);
                return trx;
            } FC_CAPTURE_AND_RETHROW()

        }

        block_id_type database::get_block_id_for_num(uint32_t block_num) const {
            block_id_type bid = find_block_id_for_num(block_num);
            FC_ASSERT(bid != block_id_type());
            return bid;
        }

        std::vector<block_id_type> database::get_block_ids_on_fork(block_id_type head_of_fork) const {
            try {
                std::pair<fork_database::branch_type, fork_database::branch_type> branches = _fork_database.fetch_branch_from(
                        head_block_id(), head_of_fork);
                if (!(branches.first.back()->previous_id() == branches.second.back()->previous_id())) {
                    edump((head_of_fork)
                                  (head_block_id())
                                  (branches.first.size())
                                  (branches.second.size()));
                    assert(branches.first.back()->previous_id() == branches.second.back()->previous_id());
                }
                std::vector<block_id_type> result;
                for (const item_ptr &fork_block : branches.second) {
                    result.emplace_back(fork_block->id);
                }
                result.emplace_back(branches.first.back()->previous_id());
                return result;
            } FC_CAPTURE_AND_RETHROW()
        }

        block_id_type database::find_block_id_for_num(uint32_t block_num) const {
            try {
                if (block_num == 0) {
                    return block_id_type();
                }

                oid<block_summary_object> bsid = block_num & 0xFFFF;
                const block_summary_object *bs = find<block_summary_object, by_id>(bsid);
                if (bs != nullptr) {
                    if (block_header::num_from_id(bs->block_id) == block_num) {
                        return bs->block_id;
                    }
                }

                //next we query the block log, Irreversible blocks are here.
                auto b = _block_log.read_block_by_num(block_num);
                if (b.valid()) {
                    return b->id();
                }

                // Finally we query the fork DB.
                std::shared_ptr<fork_item> fitem = _fork_database.fetch_block_on_main_branch_by_number(block_num);
                if (fitem) {
                    return fitem->id;
                }

                return block_id_type();
            } FC_CAPTURE_AND_RETHROW((block_num))
        }

        void database::check_free_memory(bool fore_print, uint32_t curent_block_num) {
            uint64_t free_mem = get_free_memory();
            uint64_t max_mem = get_max_memory();
            if (_shared_file_full_threshold != 0 && _shared_file_scale_rate != 0 && free_mem < ((fc::uint128_t(
                    NEWS_100_PERCENT - _shared_file_full_threshold) * max_mem) / NEWS_100_PERCENT).to_uint64()) {
                uint64_t new_max =
                        (fc::uint128_t(max_mem * _shared_file_scale_rate) / NEWS_100_PERCENT).to_uint64() + max_mem;
                wlog("Memory is almost full, increasing to ${mem}M  currnet_block_num${b}",
                     ("mem", new_max / (1024 * 1024))("b", curent_block_num));

                resize(new_max);
                uint32_t free_mb = uint32_t(get_free_memory() / (1024 * 1024));
                wlog("Free memory is now ${free}M", ("free", free_mb));
            }


        }

        const account_object &database::get_account(const account_name &name) const {
            try {
                return get<account_object, by_name>(name);
            } FC_CAPTURE_AND_RETHROW((name))
        }

        const account_object *database::find_account(const account_name &name) const {
            return find<account_object, by_name>(name);
        }

        boost::signals2::connection database::add_pre_apply_operation_handler(const apply_operation_handler_t &func,
                                                                              const news::app::abstract_plugin &plugin,
                                                                              int32_t group) {

            return any_apply_operation_handler_impl<true>(func, plugin, group);
        }

        template<bool IS_PRE_OPERATION>
        boost::signals2::connection database::any_apply_operation_handler_impl(const apply_operation_handler_t &fun,
                                                                               const news::app::abstract_plugin &plugin,
                                                                               int32_t group) {
            auto complex_func = [fun](const operation_notification &op) {
                fun(op);
            };
            if (IS_PRE_OPERATION) {

                return _pre_apply_operation_signal.connect(group, complex_func);
            } else {
                return _post_apply_operation_signal.connect(group, complex_func);
            }
        }

        boost::signals2::connection database::add_post_apply_operation_handler(const apply_operation_handler_t &func,
                                                                               const news::app::abstract_plugin &plugin,
                                                                               int32_t group) {
            return any_apply_operation_handler_impl<false>(func, plugin, group);
        }

        void database::notify_post_apply_operation(const operation_notification &note) {
            NEWS_TRY_NOTIFY(_post_apply_operation_signal, note);
        }

        void database::wipe(const fc::path &dir, const fc::path &shared_mem_dir, bool block_log) {
            close();
            chainbase::database::wipe(shared_mem_dir);
            if (block_log) {
                fc::remove_all(dir / "block_log");
                fc::remove_all(dir / "block_log.index");
            }
        }


        void database::set_flush_interval(uint32_t flush_blocks) {
            _flush_blocks = flush_blocks;
            _next_flush_block = 0;
        }

        void database::init_hardforks() {
            _hardfork_times[0] = fc::time_point_sec(NEWS_GENESIS_TIME);
            _hardfork_versions[0] = version(0, 0, 0);

            _hardfork_times[NEWS_HARDFORK_01] = fc::time_point_sec(NEWS_HARDFORK_01_TIME);
            _hardfork_versions[NEWS_HARDFORK_01] = version(0, 0, 1);

            _hardfork_times[NEWS_HARDFORK_02] = fc::time_point_sec(NEWS_HARDFORK_02_TIME);
            _hardfork_versions[NEWS_HARDFORK_02] = version(0, 0, 2);
        }

        void database::set_hardfork(uint32_t hardfork, bool now) {
            const auto &hpo = get_hardfork_property_object();

            for(uint32_t i = hpo.last_hardfork + 1; i <= hardfork && i <= NEWS_HARDFORK_NUM; i++){
                modify(hpo, [&](hardfork_property_object &obj){
                    obj.next_hardfork_version = _hardfork_versions[i];
                    obj.next_hardfork_time = head_block_time();
                });

                if(now){
                    _apply_hardfork(i);
                }
            }


        }

        const hardfork_property_object &database::get_hardfork_property_object() const {
            try{
                return get<hardfork_property_object>();
            }FC_CAPTURE_AND_RETHROW()
        }

        void database::_apply_hardfork(uint32_t hardfork) {
            elog("_apply_hardfork ${n}  at block ${m}", ("n", hardfork)("m", head_block_num()));
            switch (hardfork){
                case NEWS_HARDFORK_01:
                {
                    break;
                }
                case NEWS_HARDFORK_02:
                {
                    break;
                }
                default:{
                    elog("unkown hardfork ${n}", ("n", hardfork));
                    break;
                }
            }

            const auto &hpo = get_hardfork_property_object();
            elog("hpo size ${s}", ("s", hpo.processed_hadrdfork.size()));
            modify(hpo, [&](hardfork_property_object &obj){
                FC_ASSERT(hardfork == obj.last_hardfork + 1, "Hardfork being applied out of");
                FC_ASSERT(obj.processed_hadrdfork.size() == hardfork, "Hardfork being applied out of");
                obj.processed_hadrdfork.push_back(_hardfork_times[hardfork]);
                obj.last_hardfork = hardfork;
                obj.current_hardfork_version = _hardfork_versions[hardfork];
                FC_ASSERT(obj.processed_hadrdfork[obj.last_hardfork] == _hardfork_times[obj.last_hardfork], "Hardfork processing failed sanity check...");

            });
        }

        void database::process_hardforks() {
            try{
                const auto &hpo = get_hardfork_property_object();
//                while(_hardfork_versions[hpo.last_hardfork] < hpo.next_hardfork_version
//                        && hpo.next_hardfork_time <= head_block_time()){
//                    if(hpo.last_hardfork < NEWS_HARDFORK_NUM){
//                        _apply_hardfork(hpo.last_hardfork + 1);
//                    }
//                    else{
//                        elog("unkown hardfork exception.");
//                        throw unknown_hardfork_exception();
//                    }
//                }

                while(hpo.last_hardfork < NEWS_HARDFORK_NUM
                        && _hardfork_times[hpo.last_hardfork + 1] <= head_block_time()){
                    _apply_hardfork(hpo.last_hardfork + 1);
                }

            }FC_CAPTURE_AND_RETHROW()
        }

        bool database::has_harfork(uint32_t hardfork) const {
            return get_hardfork_property_object().processed_hadrdfork.size() > hardfork;
        }


    }//namespace chain
}//namespace news