//
// Created by boy on 18-6-14.
//

#pragma once

#include <news/chain/block.hpp>
#include <news/chain/object_types.hpp>
#include <chainbase/chainbase.hpp>


namespace news{
    namespace chain{
        using namespace boost::multi_index;
/**
    *  @brief tracks minimal information about past blocks to implement TaPOS
    *  @ingroup object
    *
    *  When attempting to calculate the validity of a transaction we need to
    *  lookup a past block and check its block hash and the time it occurred
    *  so we can calculate whether the current transaction is valid and at
    *  what time it should expire.
    */
        class block_summary_object : public chainbase::object< block_summary_object_type, block_summary_object >
        {
        public:
            template< typename Constructor, typename Allocator >
 
           // block_summary_object( Constructor&& c, allocator< Allocator > a )
			block_summary_object(Constructor&& c, Allocator && a)
 
            {
                c( *this );
            }

            block_summary_object(){};

            id_type        id;
            block_id_type  block_id;
        };

        typedef multi_index_container<
        block_summary_object,
        indexed_by<
                ordered_unique< tag< by_id >,
        member< block_summary_object, block_summary_object::id_type, &block_summary_object::id > >
        >,
        allocator< block_summary_object >
        > block_summary_index;


    }//news::chain
}//news


FC_REFLECT(news::chain::block_summary_object, (id)(block_id))
CHAINBASE_SET_INDEX_TYPE(news::chain::block_summary_object, news::chain::block_summary_index)