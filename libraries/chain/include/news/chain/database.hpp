//
// Created by boy on 18-6-11.
//

#pragma once

#include <chainbase/chainbase.hpp>
#include <news/chain/block_log.hpp>

#include <fc/filesystem.hpp>
#include <fc/reflect/reflect.hpp>
#include <news/protocol/config.hpp>
#include <news/chain/global_property_object.hpp>

namespace news{
    namespace chain{

        using namespace chainbase;

        struct open_db_args{
            fc::path        data_dir;
            fc::path        shared_mem_dir;
            uint64_t        shared_mem_size = 0;
            uint64_t        shared_file_full_threshold = 0;
            uint64_t        shared_file_scale_rate = 0;
            chainbase::database::open_flags        chainbase_flag = chainbase::database::read_write;
            uint32_t        stop_replay_at = 0;


        };




        class database : public chainbase::database{
        public:
            database();
            ~database();

            void open(const open_db_args &args);

            uint32_t    get_slot_at_time(fc::time_point_sec when);
            fc::time_point  get_slot_time(uint32_t slot_num ) const;

            const global_property_object &get_global_property_object() const;
            uint32_t                        head_block_num() const;
            fc::time_point                  head_block_time() const;

            account_name                    get_scheduled_producer(uint32_t num) const;
        private:
            void initialize_indexes();

            block_log           _block_log;

        };

    }//namespace chain
}//namesapce news

FC_REFLECT_ENUM( chainbase::database::open_flags,(read_only)(read_write))
FC_REFLECT(news::chain::open_db_args, (data_dir)(shared_mem_dir)(shared_mem_size)(shared_file_full_threshold)(shared_file_scale_rate)(chainbase_flag)(stop_replay_at))