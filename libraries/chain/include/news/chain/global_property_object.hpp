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
        class global_property_object : public chainbase::object<object_type::global_property_dynamic_obj, global_property_object>{
//        public:
            OBJECT_CTOR1(global_property_object);

            id_type             id;
            uint32_t            head_block_num = 0;
            block_id_type       head_block_id;
            fc::time_point      time;
            account_name        current_producer;

        };

        struct by_id;
        typedef multi_index_container<
        global_property_object,
        indexed_by<
                ordered_unique< tag< by_id >,
                member< global_property_object, global_property_object::id_type, &global_property_object::id > >
        >,
        allocator< global_property_object >
        > global_property_object_index;




    }//namespace chain
}//namespace news


FC_REFLECT(news::chain::global_property_object, (id)(head_block_num)(head_block_id)(time)(current_producer))

CHAINBASE_SET_INDEX_TYPE( news::chain::global_property_object, news::chain::global_property_object_index )