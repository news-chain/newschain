//
// Created by boy on 18-6-11.
//

#pragma once

#include <chainbase/chainbase.hpp>

#include <fc/filesystem.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/scoped_exit.hpp>



#include <news/base/config.hpp>
#include <news/chain/evaluator.hpp>
#include <news/chain/evaluator_registry.hpp>
#include <news/chain/block_log.hpp>
#include <news/chain/global_property_object.hpp>
#include <news/chain/block_summary_object.hpp>
#include <news/chain/fork_database.hpp>
#include <news/chain/transaction.hpp>
#include <news/chain/transaction_object.hpp>
#include <news/chain/block.hpp>
#include <news/chain/news_eveluator.hpp>

#include <news/base/operation.hpp>
#include <news/base/account_object.hpp>


namespace news{
    namespace chain{

        using namespace chainbase;
        using namespace news::base;

        class database_impl;





        struct open_db_args{
            fc::path        data_dir;
            fc::path        shared_mem_dir;
            uint64_t        shared_mem_size = 0;
            uint16_t        shared_file_full_threshold = 0;
            uint16_t        shared_file_scale_rate = 0;
            uint32_t        chainbase_flag = 0;
            uint32_t        stop_replay_at = 0;


        };

        enum validation_steps
        {
            skip_nothing                    = 0,
            skip_producer_signature         = 1 << 0,  ///< used while reindexing
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

            uint32_t                        get_slot_at_time(fc::time_point_sec when);
            fc::time_point_sec              get_slot_time(uint32_t slot_num ) const;

            const dynamic_global_property_object &get_global_property_object() const;
            uint32_t                        head_block_num() const;
            fc::time_point_sec              head_block_time() const;
            block_id_type                   head_block_id() const;

            account_name                    get_scheduled_producer(uint32_t num) const;

            signed_block                    generate_block(const fc::time_point_sec when, const account_name& producer, const fc::ecc::private_key private_key_by_signed, uint64_t skip);


            bool                            push_block(const signed_block &block, uint64_t skip);
            void                            push_transaction(const signed_transaction &trx, uint64_t skip = skip_nothing);

            void                            create_block_summary(const signed_block &b);




            ///////////////////////////////////////////////////////////////////////////////
            ////////////////////////////get             ///////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////
            fc::optional<signed_block>      fetch_block_by_number(uint32_t block_num);
            const chain_id_type             &get_chain_id() ;

            /*
           * @param trx_id
           * @return true:if exits,
           * */
            bool                            is_know_transaction(const transaction_id_type &trx_id) const;
            bool                            is_know_block(const block_id_type &id) const;
            fc::optional<signed_block>      fetch_block_by_id( const block_id_type& id )const;
            uint32_t                        last_non_undoable_block_num() const;
            const signed_transaction        get_recent_transaction(const transaction_id_type &trx_id) const;
            block_id_type                   get_block_id_for_num(uint32_t block_num) const;
            std::vector<block_id_type>      get_block_ids_on_fork(block_id_type head_of_fork)const;

            const account_object &          get_account(const account_name &name)const;
            const account_object*           find_account(const account_name &name)const;


        private:


            signed_block                    _generate_block(const fc::time_point_sec when, const account_name& producer, const fc::ecc::private_key private_key_by_signed);
            void                            initialize_indexes();

            void                            init_genesis(open_db_args args);

            void                            update_global_property_object(const signed_block &block);
            void                            update_last_irreversible_block();


            bool                            _push_block(const signed_block &block, uint64_t skip);
            void                            apply_block(const signed_block &block, uint64_t skip = skip_nothing);
            void                            _apply_block(const signed_block &block, uint64_t skip = skip_nothing);


            void                            _push_transaction(const signed_transaction &trx);
            void                            apply_transaction(const signed_transaction &trx, uint64_t skip);
            void                            _apply_transaction(const signed_transaction &trx);
            void                            apply_operation(const operation &op);
            void                            pop_block();

            void                            regists_evaluator();
            void                            clear_pending();
            void                            clear_expired_transactions();
            block_id_type                   find_block_id_for_num(uint32_t block_num) const;

            void                            check_free_memory(bool fore_print, uint32_t cuurent_block_num);



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

                assert((_pending_trx.size() == 0) || _pending_trx_session.valid());
                _pending_trx.clear();
                _pending_trx_session.reset();
                //TODO clear pending

                auto on_exit = fc::make_scoped_exit([&](){

                    for(const auto &tx : _popped_tx){
                        try{
                            if(!is_know_transaction(tx.id())){
                                _push_transaction(tx);
                            }
                        }catch(const fc::exception &e){
                            //TODO throw?
                        }
                    }


                    for(auto &t : old_input){
                        try{
                            bool find = is_know_transaction(t.id());
                            if(!find){
                                _push_transaction(std::move(t));
                            }
                        }catch (const fc::exception &e){
                            elog("without_pengding_transactions. ${trx}, ${e}", ("trx", t)("e", e.to_detail_string()));
                        }
                        catch (...){
                            elog("unhandle without_pengding_transactions");
                        }
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
            fc::optional< chainbase::database::session >    _pending_trx_session;
            std::deque<signed_transaction>                  _popped_tx;

            std::unique_ptr< database_impl > _my;

            uint16_t                                        _shared_file_full_threshold;
            uint16_t                                        _shared_file_scale_rate;
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



