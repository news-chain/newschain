//
// Created by boy on 18-6-11.
//

#include <news/chain/database.hpp>
#include <fc/scoped_exit.hpp>
namespace news{
    namespace chain{

        namespace detail{
/**
 * Set the skip_flags to the given value, call callback,
 * then reset skip_flags to their previous value after
 * callback is done.
 */
//            template< typename Lambda >
//            void with_skip_flags(
//                    database& db,
//                    uint32_t skip_flags,
//                    Lambda callback )
//            {
//                node_property_object& npo = db.node_properties();
//                skip_flags_restorer restorer( npo, npo.skip_flags );
//                npo.skip_flags = skip_flags;
//                callback();
//                return;
//            }


        }//news::chain::detail





        database::database() {

        }

        database::~database() {

        }

        void database::open(const news::chain::open_db_args &args) {
            chainbase::database::open(args.shared_mem_dir, args.chainbase_flag, args.shared_mem_size);

            _block_log.open(args.data_dir / "block_log");

            initialize_indexes();
            init_genesis(args);

            auto log_header = _block_log.head();
            with_write_lock([&](){
                undo_all();
                FC_ASSERT( revision() == head_block_num(), "Chainbase revision does not match head block num", ("rev", revision())("head_block ", head_block_num()));
                //TODO do_validate_invariants

            });


            if(head_block_num()){
                auto head_block = _block_log.read_block_by_num(head_block_num());
                FC_ASSERT(head_block.valid() && head_block_id() == head_block->id(), "Chain state does not match block log. Please reindex blockchain.");
                _fork_database.start_block(*head_block);
            }

            //TODO init hardforks

        }

        void database::initialize_indexes() {
//            add_index<>();
            //TODO add index
            add_index<dynamic_global_property_object_index>();
            add_index<block_summary_index>();
            add_index<transaction_obj_index>();
        }

        uint32_t database::get_slot_at_time(fc::time_point_sec when) {
            fc::time_point_sec first_slot_time = get_slot_time(1);
            if(when < first_slot_time){
                return 0;
            }
            return (when - first_slot_time).to_seconds() / NEWS_BLOCK_INTERVAL + 1;
        }

        fc::time_point database::get_slot_time(uint32_t slot_num) const {
            if(slot_num == 0){
                return fc::time_point_sec();
            }

            const dynamic_global_property_object &gpo = get_global_property_object();
            if(gpo.head_block_num == 0){
                fc::time_point_sec gensis_time = gpo.time;
                return  (gensis_time + slot_num * NEWS_BLOCK_INTERVAL);
            }

            int64_t head_block_abs_slot = gpo.time.sec_since_epoch() / NEWS_BLOCK_INTERVAL;
            fc::time_point_sec head_slot_time(head_block_abs_slot * NEWS_BLOCK_INTERVAL);
            return head_slot_time + (slot_num * NEWS_BLOCK_INTERVAL);

        }


        const dynamic_global_property_object &database::get_global_property_object() const {
            try {
                return get<dynamic_global_property_object>();
            }FC_CAPTURE_AND_RETHROW()
        }

        uint32_t database::head_block_num() const {
            return get_global_property_object().head_block_num;
        }

        fc::time_point database::head_block_time() const {
            return get_global_property_object().time;
        }

        block_id_type database::head_block_id() const {
            return get_global_property_object().head_block_id;
        }


        account_name database::get_scheduled_producer(uint32_t num) const {
            return 0;
        }

        signed_block database::generate_block(const fc::time_point_sec when, const account_name &producer,
                                              const fc::ecc::private_key private_key_by_signed, uint64_t skip) {

            //
            signed_block pengding_block;

            pengding_block.timestamp = when;
            pengding_block.previous = head_block_id();
            pengding_block.transaction_merkle_root = pengding_block.caculate_merkle_root();
            pengding_block.producer = producer;
            if(!(skip & skip_producer_signature)){
                pengding_block.sign(private_key_by_signed);
            }

            if(!(skip & skip_block_size_check)){
                FC_ASSERT(fc::raw::pack_size(pengding_block) <= NEWS_MAX_BLOCK_SIZE);
            }


            //TODO push block
            push_block(pengding_block, skip);

            return pengding_block;
        }

        signed_block database::generate_block(const fc::time_point_sec when, const account_name &producer,
                                              const fc::ecc::private_key private_key_by_signed) {
            return signed_block();
        }

        void database::init_genesis(open_db_args args) {

            if(!find<dynamic_global_property_object>()){
                with_write_lock([&](){
                    create<dynamic_global_property_object>([](dynamic_global_property_object &obj){
                        obj.time = NEWS_GENESIS_TIME;
                    });
                });
            }

        }

        void database::update_global_property_object(const signed_block &block) {
            const auto &gpo = get_global_property_object();
            modify(gpo, [block](dynamic_global_property_object &obj){
                obj.time = block.timestamp;
                obj.head_block_num = block.block_num();
                obj.head_block_id = block.id();
            });
        }

        void database::update_last_irreversible_block() {
            static_assert(IRREVERSIBLE_BLOCK_NUM > 0, "irreversible_block_num must be nonzero.");


            //update



            const auto &gpo = get_global_property_object();
            commit(gpo.last_irreversible_block_num);
            if(!(_skip_flags & skip_block_log)){
                uint32_t log_head_num = 0;
                const  auto &temp_head = _block_log.head();
                if(temp_head){
                    log_head_num = temp_head->block_num();
                }
                if(log_head_num < gpo.last_irreversible_block_num){
                    while(log_head_num < gpo.last_irreversible_block_num){
                        shared_ptr<fork_item> fitem = _fork_database.fetch_block_on_main_branch_by_number(log_head_num + 1);
                        FC_ASSERT(fitem, "Current fork in the fork database does not contain the last_irreversible_block");
                        _block_log.append(fitem->data);
//                        elog("block log append block ${b}", ("b", fitem->data.block_num()));
                        log_head_num++;
                    }
                }
            }

            if(gpo.head_block_num > IRREVERSIBLE_BLOCK_NUM){
                modify(gpo, [&](dynamic_global_property_object &obj){
                    obj.last_irreversible_block_num = head_block_num() - IRREVERSIBLE_BLOCK_NUM;
                });
            }



        }


        void database::apply_block(const signed_block &block, uint64_t skip) {
            try {
                auto block_num = block.block_num();
                //TODO checkpoints

                with_skip_flags(skip, [&](){
                    _apply_block(block, skip);
                });

                //TODO flush chainbase
            }FC_CAPTURE_AND_RETHROW()
        }

        void database::_apply_block(const signed_block &block, uint64_t skip) {
            try {
                auto merkle_root = block.caculate_merkle_root();
                try {
                    FC_ASSERT(merkle_root == block.transaction_merkle_root, "merkle check failed",("new_block merkle root", block.producer_signature)("caculate merkle root ", merkle_root));
                }catch (fc::assert_exception &e){
                    //TODO catch exception
                    elog("_apply_block ", ("e", e.what()));
                }

                auto block_size = fc::raw::pack_size(block);
                if(block_size < NEWS_MIN_BLOCK_SIZE){
                   elog("block size si too small ", ("block_num", block.block_num())("block_size", block_size));
//                   elog("block : ${b}", ("b", block.timestamp));
                }


                update_last_irreversible_block();
                update_global_property_object(block);


            }FC_CAPTURE_AND_RETHROW()
        }


        bool database::push_block(const signed_block &block, uint64_t skip) {
            bool result = false;
            with_skip_flags(skip, [&](){
                    without_pengding_transactions([&](){
                        try{
                            result = _push_block(block, skip);
                        }FC_CAPTURE_AND_RETHROW()
                    });
            });
            return false;
        }

        bool database::_push_block(const signed_block &block, uint64_t skip) {
//            elog("_push block  ${b}", ("b", block));
            try{
                if(!(skip & skip_fork_db)){
                    shared_ptr<fork_item> new_block = _fork_database.push_block(block);
                    //TODO find producer?

                    //If the head block from the longest chain does not build off of the current head, we need to switch forks.
                    if(new_block->data.previous != head_block_id()){

                        //If the newly pushed block is the same height as head, we get head back in new_head
                        //Only switch forks if new_head is actually higher than head
                        if(new_block->data.block_num() > head_block_num()){


                            wlog("switching to for : ${id}", ("id", new_block->data.id()));
                            auto branches = _fork_database.fetch_branch_from(new_block->data.id(), head_block_id());

                            while(head_block_id() != branches.second.back()->data.previous)
                                pop_block();

//                        push all blocks on the new block
                            for(auto ritr = branches.first.rbegin(); ritr != branches.first.rend(); ritr++){
                                ilog("push blocks from for ${n} ${id}", ("n", (*ritr)->data.block_num())("id", (*ritr)->data.id()));

                                fc::optional<fc::exception> exception;
                                try {
                                    auto session = start_undo_session();
                                    apply_block((*ritr)->data, skip);
                                    session.push();
                                }catch (const fc::exception &e){
                                    exception = e;
                                }

                                if(exception){
                                    wlog( "exception thrown while switching forks ${e}", ("e",exception->to_detail_string() ) );
                                    // remove the rest of branches.first from the fork_db, those blocks are invalid
                                    while(ritr != branches.first.rend()){
                                        _fork_database.remove((*ritr)->data.id());
                                        ritr++;
                                    }
                                    _fork_database.set_head(branches.second.front());

                                    // pop all blocks from the bad fork
                                    while(head_block_id() != branches.second.back()->data.previous){
                                        pop_block();
                                    }

                                    for(auto itr = branches.second.rbegin(); itr != branches.second.rend(); itr++){
                                        auto session = start_undo_session();
                                        apply_block((*itr)->data, skip);
                                        session.push();
                                    }
                                    throw  *exception;
                                }

                            }
                            return true;
                        }else{
                            return false;
                        }
                    }
                }

                try {
                    auto session = start_undo_session();
                    apply_block(block, skip);
                    session.push();
                }catch (const fc::exception &e){
                    elog("Failed to push new block:\n${e}", ("e", e.to_detail_string()));
                    _fork_database.remove(block.id());
                    throw ;
                }
                return false;
            }FC_CAPTURE_AND_RETHROW()

        }

        void database::create_block_summary(const signed_block &b) {
            try {
                oid<block_summary_object> sid(b.block_num() & 0xffff );
                modify(get< block_summary_object >(sid), [&](block_summary_object &obj){
                    obj.block_id = b.id();
                });
            }FC_CAPTURE_AND_RETHROW()
        }

        bool database::is_know_transaction(const transaction_id_type &trx_id) {
            try {
                const auto &trx_itr = get_index<transaction_obj_index>().indices().get<by_trx_id>();
                return trx_itr.find(trx_id) != trx_itr.end();
            }FC_CAPTURE_AND_RETHROW()
        }

        void database::push_transaction(const signed_transaction &trx, uint64_t skip) {
            try {
                try {
//                    FC_ASSERT(fc::raw::pack(trx) <= NEWS_MAX_BLOCK_SIZE - 256);
                    set_producing(true);

                    with_skip_flags(skip, [&](){
                        _push_transaction(trx);
                    });

                    set_producing(false);
                }
                catch (...){
                    set_producing(false);
                }

            }FC_CAPTURE_AND_RETHROW()
        }

        void database::_push_transaction(const signed_transaction &trx) {
           //TODO is valid
//            _pending_trx = start_undo_session(true);

//            auto temp_session = start_undo_session();

        }

        void database::pop_block() {
            try {
                _pending_tx_session.reset();
                auto head_id = head_block_id();
                fc::optional<signed_block> head_block = fetch_block_by_id(head_id);
                FC_ASSERT(head_block.valid(), "there is no block to pop");
                _fork_database.pop_block();
                undo();

                //TODO record poped transaction , insert next blocks?
//                _popped_tx.insert(_popped_tx.begin(), head_block->transactions.begin())

            }FC_CAPTURE_AND_RETHROW()

        }

        fc::optional<signed_block> database::fetch_block_by_id(const block_id_type &id) const {
            try {
                auto b = _fork_database.fetch_block(id);
                if(!b){
                    auto temp = _block_log.read_block_by_num(news::chain::block_header::num_from_id(id));
                    if(temp && temp->id() == id){
                        return temp;
                    }
                    temp.reset();
                    return temp;
                }
                return b->data;
            }FC_CAPTURE_AND_RETHROW()
        }

        uint32_t database::reindex(const open_db_args &args) {
            try {
                ilog("reindex blockchain");
                wipe(args.shared_mem_dir);
                open(args);
                _fork_database.reset();

                auto start = fc::time_point::now();

                uint64_t skip_flas =
                        skip_producer_signature         |
                        skip_transaction_signatures     |
                        skip_tapos_check                |
                        skip_merkle_check               |
                        skip_producer_schedule_check    |
                        skip_authority_check            |
                        skip_validate                   |
                        skip_validate_invariants        |
                        skip_block_log;


                with_write_lock([&](){
                    _block_log.set_locking(false);
                    auto itr = _block_log.read_block(0);
                    auto last_block_num = _block_log.read_head().block_num();
                    //TODO stop at block num?
                    while(itr.first.block_num() != last_block_num){
                        auto current_block_num = itr.first.block_num();
                        if(current_block_num % 10000 == 0){
                            std::cerr << itr.first.block_num() << std::endl;
                        }
                        apply_block(itr.first, skip_flas);

                        itr = _block_log.read_block( itr.second );
                    }
                });



                auto end = fc::time_point::now();
                elog("Done reindexing blcoks, elapsed time ${t} sec", ("t", (double)(end -start).count() / 1000000.0));

                //TODO return  stop at block num
                return _block_log.read_head().block_num();
            }FC_CAPTURE_AND_RETHROW()

        }


    }//namespace chain
}//namespace news