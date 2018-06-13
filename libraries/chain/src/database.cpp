//
// Created by boy on 18-6-11.
//

#include <news/chain/database.hpp>

namespace news{
    namespace chain{


        database::database() {

        }

        database::~database() {

        }

        void database::open(const news::chain::open_db_args &args) {
            chainbase::database::open(args.shared_mem_dir, args.chainbase_flag, args.shared_mem_size);

            _block_log.open(args.data_dir / "block_log");
            initialize_indexes();
            auto header = _block_log.head();
        }

        void database::initialize_indexes() {
//            add_index<>();
            //TODO add index
            add_index<global_property_object_index>();
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

            const global_property_object &gpo = get_global_property_object();
            if(gpo.head_block_num == 0){
                fc::time_point_sec gensis_time = gpo.time;
                return  (gensis_time + slot_num * NEWS_BLOCK_INTERVAL);
            }

            int64_t head_block_abs_slot = gpo.time.sec_since_epoch() / NEWS_BLOCK_INTERVAL;
            fc::time_point_sec head_slot_time(head_block_abs_slot * NEWS_BLOCK_INTERVAL);
            return head_slot_time + (slot_num * NEWS_BLOCK_INTERVAL);

        }


        const global_property_object &database::get_global_property_object() const {
            try {
                return get<global_property_object>();
            }FC_CAPTURE_AND_RETHROW()
        }

        uint32_t database::head_block_num() const {
            return get_global_property_object().head_block_num;
        }

        fc::time_point database::head_block_time() const {
            return get_global_property_object().time;
        }

        account_name database::get_scheduled_producer(uint32_t num) const {
            return 0;
        }


    }//namespace chain
}//namespace news