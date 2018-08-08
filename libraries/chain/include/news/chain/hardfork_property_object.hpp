//
// Created by boy on 18-8-6.
//

#pragma once

#include <news/base/version.hpp>
#include <news/base/types.hpp>
#include <news/chain/object_types.hpp>

#include <boost/multi_index/member.hpp>
namespace news{
    namespace chain{

        using namespace news::base;
        using namespace chainbase;
        using namespace boost::multi_index;


        class hardfork_property_object : public chainbase::object<hardfork_property_object_type, hardfork_property_object>{
        public:
            template< typename Constructor, typename Allocator >
            hardfork_property_object( Constructor&& c, chainbase::allocator< Allocator > a )
            :processed_hadrdfork(a)

            {
                c( *this );

            }

            id_type             id;

            using t_hardfork = chainbase::t_vector <fc::time_point_sec>;


            t_hardfork                  processed_hadrdfork;
            uint32_t                    last_hardfork = 0;
            news::base::version         current_hardfork_version;
            news::base::version         next_hardfork_version;
            fc::time_point_sec          next_hardfork_time;

        };

        typedef multi_index_container<
        hardfork_property_object,
        indexed_by<
                ordered_unique< member< hardfork_property_object, hardfork_property_object::id_type, &hardfork_property_object::id > >
        >,
        chainbase::allocator< hardfork_property_object >
        > hardfork_property_index;

    }
}


FC_REFLECT(news::chain::hardfork_property_object, (id)(processed_hadrdfork)(last_hardfork)(current_hardfork_version)(next_hardfork_time))

CHAINBASE_SET_INDEX_TYPE( news::chain::hardfork_property_object, news::chain::hardfork_property_index )