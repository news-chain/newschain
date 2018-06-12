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

            auto header = _block_log.head();
        }

        void database::initialize_indexes() {
//            add_index<>();
            //TODO add index
        }



    }//namespace chain
}//namespace news