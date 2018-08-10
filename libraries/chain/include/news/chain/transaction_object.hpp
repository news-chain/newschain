//
// Created by boy on 18-6-14.
//

#pragma once

#include <chainbase/chainbase.hpp>
#include <news/chain/object_types.hpp>
#include <fc/time.hpp>
#include <news/base/types.hpp>


#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/ordered_index.hpp>
namespace news{
    namespace chain{

        using namespace boost::multi_index;
        using namespace news::base;
        using namespace chainbase;
        typedef buffer_type     packed_trx_buffer;

        class transaction_object : public object<transaction_object_type, transaction_object>{
            transaction_object() = delete;
        public:
//            CHAINBASE_DEFAULT_CONSTRUCTOR(transaction_object)
            template<typename Constructor, typename Allocator>
            transaction_object( Constructor&& c, chainbase::allocator< Allocator > a):packed_trx(a) { c(*this); }

            id_type                                 id;
            fc::time_point_sec                      expiration;
            transaction_id_type                     trx_id;
            packed_trx_buffer                       packed_trx;

        };


        struct by_expiration;
        struct by_trx_obj_id;
        struct by_id;

        typedef multi_index_container<
                transaction_object,
                indexed_by<
                        ordered_unique< tag<by_id>, member<transaction_object, transaction_object::id_type, &transaction_object::id>>,
//                        hashed_unique< tag<by_trx_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, transaction_id_type, trx_id), std::hash<transaction_id_type> >,
                        ordered_unique< tag<by_trx_obj_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, transaction_id_type, trx_id)>,
//                        ordered_unique< tag<by_trx_id>, member<transaction_object, transaction_id_type, &transaction_object::trx_id>>,
                        ordered_non_unique< tag< by_expiration>,
//                                composite_key<transaction_object,
//                                      member<transaction_object, fc::time_point_sec, &transaction_object::expiration> ,
//                                      member<transaction_object, transaction_object::id_type, &transaction_object::id>
//                                >
                                member<transaction_object, fc::time_point_sec, &transaction_object::expiration >
                        >
                        >,
                        chainbase::allocator <transaction_object>
        > transaction_obj_index;




    }//news::chain
}//news


FC_REFLECT(news::chain::transaction_object, (id)(expiration)(trx_id)(packed_trx))
CHAINBASE_SET_INDEX_TYPE(news::chain::transaction_object, news::chain::transaction_obj_index)

