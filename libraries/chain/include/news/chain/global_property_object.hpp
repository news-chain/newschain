//
// Created by boy on 18-6-13.
//

#pragma once

#include <chainbase/chainbase.hpp>
#include <news/chain/object_types.hpp>
#include <news/protocol/types.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
namespace news{
    namespace chain{

        using namespace news::protocol;
        using namespace chainbase;
//        using namespace boost::mu
        using namespace boost::multi_index;
        class dynamic_global_property_object : public chainbase::object<object_type::global_property_dynamic_obj, dynamic_global_property_object>{
        public:
            CHAINBASE_DEFAULT_CONSTRUCTOR(dynamic_global_property_object);

            id_type             id;
            uint32_t            head_block_num = 0;
            block_id_type       head_block_id;
            fc::time_point      time;
            account_name        current_producer;
            uint32_t            last_irreversible_block_num = 0;
        };

        struct by_id;
        typedef multi_index_container<
                dynamic_global_property_object,
        indexed_by<
                ordered_unique< tag< by_id >,
                member< dynamic_global_property_object, dynamic_global_property_object::id_type, &dynamic_global_property_object::id > >
        >,
        allocator< dynamic_global_property_object >
        > dynamic_global_property_object_index;




    }//namespace chain
}//namespace news


FC_REFLECT(news::chain::dynamic_global_property_object, (id)(head_block_num)(head_block_id)(time)(current_producer))

CHAINBASE_SET_INDEX_TYPE( news::chain::dynamic_global_property_object, news::chain::dynamic_global_property_object_index )