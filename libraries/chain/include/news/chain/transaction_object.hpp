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


namespace news{
    namespace chain{

        using namespace boost::multi_index;

        class transaction_object : public chainbase::object<transaction_object_type, transaction_object>{
            transaction_object() = delete;
        public:
            CHAINBASE_DEFAULT_CONSTRUCTOR(transaction_object)

            id_type                                 id;
            fc::time_point_sec                      expiration;
            news::base::transaction_id_type     trx_id;
        };


        struct by_expiration;
        struct by_trx_id;
        struct by_id;

        typedef multi_index_container<
                transaction_object,
                indexed_by<
                        ordered_unique< tag<by_id>, member<transaction_object, transaction_object::id_type, &transaction_object::id>>,
                        hashed_unique< tag<by_trx_id>, member<transaction_object, news::base::transaction_id_type, &transaction_object::trx_id>, std::hash<news::base::transaction_id_type> >,
                        ordered_non_unique< tag< by_expiration>, member<transaction_object, fc::time_point_sec, &transaction_object::expiration> >
                        >,
                        chainbase::allocator <transaction_object>
        > transaction_obj_index;




    }//news::chain
}//news


FC_REFLECT(news::chain::transaction_object, (id)(expiration)(trx_id))
CHAINBASE_SET_INDEX_TYPE(news::chain::transaction_object, news::chain::transaction_obj_index)

