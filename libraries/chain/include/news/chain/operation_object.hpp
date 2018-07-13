//
// Created by boy on 18-7-3.
//

#pragma once

#include <news/chain/object_types.hpp>
//#include <chainbase/chainbase.hpp>
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
            operation_object( Constructor&& c, chainbase::allocator<Allocator> a ):op_packed(a)
            {
                c( *this );
            }
            id_type                 id;
            transaction_id_type     trx_id;
            uint32_t                block = 0;
            uint32_t                trx_in_block = 0;
            fc::time_point_sec      timestamp;
            //TODO buffer
            buffer_type             op_packed;
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

        struct by_account;
        typedef multi_index_container<
                account_history_object,
                indexed_by<
                        ordered_unique< tag<by_id>,
                        member<account_history_object, account_history_object::id_type, &account_history_object::id>
                        >,
                        ordered_unique< tag<by_account>,
                              composite_key<account_history_object,
                                      member<account_history_object, account_name, &account_history_object::name>,
                                      member<account_history_object, uint32_t, &account_history_object::sequence>
                              >,
                                composite_key_compare<std::less<account_name>, std::greater<uint32_t>>
                        >
                        >,chainbase::allocator<account_history_object>
                > account_history_obj_index;

    }}//news::chain



FC_REFLECT(news::chain::operation_object, (id)(trx_id)(block)(trx_in_block)(timestamp)(op_packed))
FC_REFLECT(news::chain::account_history_object, (id)(name)(sequence)(op_id))


CHAINBASE_SET_INDEX_TYPE(news::chain::operation_object, news::chain::operation_obj_index)
CHAINBASE_SET_INDEX_TYPE(news::chain::account_history_object, news::chain::account_history_obj_index)
