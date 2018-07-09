//
// Created by boy on 18-7-3.
//

#pragma once

#include <news/chain/object_types.hpp>
#include <chainbase/chainbase.hpp>
#include <news/base/types.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>

namespace news{
    namespace chain{

        using namespace chainbase;
        using namespace news::base;
        using namespace boost::multi_index;

        class operation_object : public object<operation_object_type, operation_object>{
        public:
            template< typename Constructor, typename Allocator >
            operation_object( Constructor&& c, Allocator &&a )
            {
                c( *this );
            }
            id_type                 id;
            transaction_id_type     trx_id;
            uint32_t                block = 0;
            uint32_t                trx_in_block = 0;
            fc::time_point_sec      timestamp;
            //TODO buffer
        };




        struct by_id;
        typedef boost::multi_index_container<
                operation_object,
                indexed_by<
                        ordered_unique< tag<by_id>,
                                member<operation_object, operation_object::id_type, &operation_object::id>
                        >
                >,chainbase::allocator <operation_object>
                > operation_obj_index;



        class account_history_object : public object<account_history_object_type, account_history_object>{
        public:
            template< typename Constructor, typename Allocator >
            account_history_object( Constructor&& c, Allocator &&a )
            {
                c( *this );
            }

            id_type                 id;
            account_name            name;
            uint32_t                sequence = 0;
            oid<operation_object>   op_id;
        };

        typedef multi_index_container<
                account_history_object,
                indexed_by<
                        ordered_unique< tag<by_id>,
                        member<account_history_object, account_history_object::id_type, &account_history_object::id>
                        >
                        >,chainbase::allocator<account_history_object>
                > account_hsitory_obj_index;

    }}//news::chain



FC_REFLECT(news::chain::operation_object, (id)(trx_id)(block)(trx_in_block)(timestamp))
FC_REFLECT(news::chain::account_history_object, (id)(name)(sequence)(op_id))