//
// Created by boy on 18-6-18.
//

#pragma once

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>

#include <chainbase/chainbase.hpp>
#include <news/base/types.hpp>
#include <news/base/asset.hpp>
#include <news/chain/object_types.hpp>
#include <news/base/authority.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace news{
    namespace base{

        using namespace boost::multi_index;
        using boost::multi_index_container;


    class account_object : public chainbase::object<news::chain::account_object_type, account_object>{
    public:
        template<typename Constructor, typename Allocator>
        account_object( Constructor&& c, Allocator&&  alloc) { c(*this); }
        id_type                         id;
        account_name                    name;
        account_name                    creator;
        asset                           balance;
        fc::time_point                  create_time;
    };



    class account_authority_object : public chainbase::object<news::chain::account_authority_type, account_authority_object>{
    public:
        template<typename Constructor, typename Allocator>
        account_authority_object( Constructor&& c, Allocator&&  alloc)
        {
            c(*this);
        }
        id_type                         id;
        account_name                    name;
        shared_authority                posting;
        shared_authority                owner;
    };


    struct by_id;
    struct by_name;
    typedef multi_index_container<
            account_object,
            indexed_by<
                        ordered_unique<tag<by_id>,
                                member<account_object, account_object::id_type, &account_object::id>
                        >,
                        ordered_unique<tag<by_name>,
                                member<account_object, account_name, &account_object::name>
                        >
                    >
            ,chainbase::allocator <account_object>
    >
    account_object_index;



    typedef multi_index_container<
            account_authority_object,
            indexed_by<
                    ordered_unique<tag<by_id>,
                            member<account_authority_object, account_authority_object::id_type, &account_authority_object::id>
                            >,
                    ordered_unique<tag<by_name>,
                            member<account_authority_object, account_name, &account_authority_object::name>
                    >
            >,
            chainbase::allocator <account_authority_object>
    > account_authority_index;


    }//news::base
}//news


FC_REFLECT(news::base::account_object, (id)(name)(creator)(balance)(create_time))
CHAINBASE_SET_INDEX_TYPE(news::base::account_object, news::base::account_object_index)

FC_REFLECT(news::base::account_authority_object, (id)(name)(posting)(owner))
CHAINBASE_SET_INDEX_TYPE(news::base::account_authority_object, news::base::account_authority_index)

