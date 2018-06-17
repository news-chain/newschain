//
// Created by boy on 18-6-11.
//

#pragma once

#include <chainbase/chainbase.hpp>

#include <fc/filesystem.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/scoped_exit.hpp>


#include <news/protocol/config.hpp>
#include <news/chain/block_log.hpp>
#include <news/chain/global_property_object.hpp>
#include <news/chain/block_summary_object.hpp>
#include <news/chain/fork_database.hpp>
#include <news/chain/transaction.hpp>
#include <news/chain/transaction_object.hpp>
#include <news/chain/block.hpp>

namespace news{
    namespace chain{

        using namespace chainbase;

        struct open_db_args{
            fc::path        data_dir;
            fc::path        shared_mem_dir;
            uint64_t        shared_mem_size = 0;
            uint64_t        shared_file_full_threshold = 0;
            uint64_t        shared_file_scale_rate = 0;
            uint32_t        chainbase_flag = 0;
            uint32_t        stop_replay_at = 0;


        };

        enum validation_steps
        {
            skip_nothing                    = 0,
            skip_producer_signature          = 1 << 0,  ///< used while reindexing
            skip_transaction_signatures     = 1 << 1,  ///< used by non-witness nodes
            skip_transaction_dupe_check     = 1 << 2,  ///< used while reindexing
            skip_fork_db                    = 1 << 3,  ///< used while reindexing
            skip_block_size_check           = 1 << 4,  ///< used when applying locally generated transactions
            skip_tapos_check                = 1 << 5,  ///< used while reindexing -- note this skips expiration check as well
            skip_authority_check            = 1 << 6,  ///< used while reindexing -- disables any checking of authority on transactions
            skip_merkle_check               = 1 << 7,  ///< used while reindexing
            skip_undo_history_check         = 1 << 8,  ///< used while reindexing
            skip_producer_schedule_check    = 1 << 9,  ///< used while reindexing
            skip_validate                   = 1 << 10, ///< used prior to checkpoint, skips validate() call on transaction
            skip_validate_invariants        = 1 << 11, ///< used to skip database invariant check on block application
            skip_undo_block                 = 1 << 12, ///< used to skip undo db on reindex
            skip_block_log                  = 1 << 13  ///< used to skip block logging on reindex
        };


        class database : public chainbase::database{
        public:
            database();
            ~database();

            void                            open(const open_db_args &args);
            uint32_t                            reindex(const open_db_args &args);

            bool                            is_producing()const{return _is_producing;}
            void                            set_producing(bool p){_is_producing = p;}

            uint32_t    get_slot_at_time(fc::time_point_sec when);
            fc::time_point  get_slot_time(uint32_t slot_num ) const;

            const dynamic_global_property_object &get_global_property_object() const;
            uint32_t                        head_block_num() const;
            fc::time_point                  head_block_time() const;
            block_id_type                   head_block_id() const;

            account_name                    get_scheduled_producer(uint32_t num) const;

            signed_block                    generate_block(const fc::time_point_sec when, const account_name& producer, const fc::ecc::private_key private_key_by_signed, uint64_t skip);
            signed_block                    generate_block(const fc::time_point_sec when, const account_name& producer, const fc::ecc::private_key private_key_by_signed);

            bool                            push_block(const signed_block &block, uint64_t skip);
            void                            push_transaction(const signed_transaction &trx, uint64_t skip = skip_nothing);

            void                            create_block_summary(const signed_block &b);
        private:
            void                            initialize_indexes();

            void                            init_genesis(open_db_args args);

            void                            update_global_property_object(const signed_block &block);
            void                            update_last_irreversible_block();


            bool                            _push_block(const signed_block &block, uint64_t skip);
            void                            apply_block(const signed_block &block, uint64_t skip = skip_nothing);
            void                            _apply_block(const signed_block &block, uint64_t skip = skip_nothing);

            bool                            is_know_transaction(const transaction_id_type &trx_id);
            void                            _push_transaction(const signed_transaction &trx);
            void                            pop_block();
            fc::optional<signed_block>      fetch_block_by_id( const block_id_type& id )const;
            //
            template<typename Function>
            auto with_skip_flags(uint64_t flags, Function   &&ff){
                auto on_exit = fc::make_scoped_exit([old_flags = _skip_flags, this](){
                    _skip_flags = old_flags;
                });
                _skip_flags = flags;
                return ff();
            }

            //clear _pending trx
            template <typename Function>
            auto without_pengding_transactions(Function &&f){
                std::vector<signed_transaction> old_input;
                old_input = std::move(_pending_trx);
                //TODO clear pending
                auto on_exit = fc::make_scoped_exit([&](){
                    for(auto &t : old_input){
                        try{
                            if(!is_know_transaction(t.id())){
                                _push_transaction(std::move(t));
                            }
                        }catch (...){}
                    }
                });
                return f();
            }




        private:

            block_log                                       _block_log;
            fork_database                                   _fork_database;
            std::vector<signed_transaction>                 _pending_trx;
            uint64_t                                        _skip_flags = skip_nothing;
            bool                                            _is_producing = false;
            fc::optional< chainbase::database::session >    _pending_tx_session;
            std::deque<signed_transaction>                  _popped_tx;
        };

    }//namespace chain
}//namesapce news

FC_REFLECT(news::chain::open_db_args, (data_dir)(shared_mem_dir)(shared_mem_size)(shared_file_full_threshold)(shared_file_scale_rate)(chainbase_flag)(stop_replay_at))

FC_REFLECT_ENUM(news::chain::validation_steps,
                (skip_nothing)
                        (skip_producer_signature)
                        (skip_transaction_signatures)
                        (skip_transaction_dupe_check)
                        (skip_fork_db)
                        (skip_block_size_check)
                        (skip_tapos_check)
                        (skip_authority_check)
                        (skip_merkle_check)
                        (skip_undo_history_check)
                        (skip_producer_schedule_check)
                        (skip_validate)
                        (skip_validate_invariants)
                        (skip_undo_block)
                        (skip_block_log))