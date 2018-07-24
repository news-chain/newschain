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
 

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>


namespace news {
	namespace base { 

		using namespace boost::multi_index;
		using boost::multi_index_container;


		class comment_object : public chainbase::object<news::chain::comment_object_type, comment_object> {
		public:
			//        CHAINBASE_DEFAULT_CONSTRUCTOR(account_object)
			template<typename Constructor, typename Allocator>
			comment_object(Constructor&& c, Allocator&&  alloc) :title(alloc), 
				body(alloc), permlink(alloc), metajson(alloc) { c(*this); }
			id_type                         id;  
			account_name                    author;
			chainbase::shared_string        title;
			chainbase::shared_string        body;
			chainbase::shared_string        permlink;
			chainbase::shared_string        metajson;
			uint64_t up;  
			uint64_t down;  
			fc::time_point                  create_time;

		};

		 

		struct by_id; 
		struct by_permlink;
		struct by_author_time;;
		typedef multi_index_container<
			comment_object,
			indexed_by<
			ordered_unique<tag<by_id>,
			member<comment_object, comment_object::id_type, &comment_object::id> 
					 >,
			 
			ordered_unique< tag< by_permlink >,
			composite_key< comment_object,
			member<comment_object, chainbase::shared_string, &comment_object::permlink>
			>,
			composite_key_compare<strcmp_less>
			>,
			 

			ordered_non_unique< tag< by_author_time >, 
			composite_key< comment_object,
			member< comment_object, account_name, &comment_object::author >,
			member< comment_object, fc::time_point, &comment_object::create_time >
			>,
			composite_key_compare< std::less< account_name>, std::less<fc::time_point> >
			>
			>,  

			chainbase::allocator <comment_object>
		>
			comment_object_index;




		class comment_vote_object : public chainbase::object<news::chain::comment_vote_object_type, comment_vote_object> {
		public:
			//        CHAINBASE_DEFAULT_CONSTRUCTOR(account_object)
			template<typename Constructor, typename Allocator>
			comment_vote_object(Constructor&& c, Allocator&&  alloc): permlink(alloc){
				c(*this);
			}
			id_type                         id; 
			account_name                    voter;   
			chainbase::shared_string        permlink; 
			int								ticks;
			fc::time_point                  create_time;
		};
		  
		struct by_vote_id;
		struct by_vote_permlink; 

		typedef multi_index_container<
			comment_vote_object, 
			indexed_by<
			ordered_unique<tag<by_vote_id>,
			member<comment_vote_object, comment_vote_object::id_type, &comment_vote_object::id>
					>,
			ordered_unique< tag< by_vote_permlink >,
			composite_key< comment_vote_object,
			member<comment_vote_object, chainbase::shared_string, &comment_vote_object::permlink>
						>,
			composite_key_compare<strcmp_less>
						>
			>, 
 
			chainbase::allocator <comment_vote_object>
			>
			comment_vote_object_index;  
		 
		 

	}//news::base
}//news
 


FC_REFLECT(news::base::comment_object, (id)(author)(title)(body)(permlink)(metajson)(create_time))
CHAINBASE_SET_INDEX_TYPE(news::base::comment_object, news::base::comment_object_index) 

 
FC_REFLECT(news::base::comment_vote_object, (id)(voter)(permlink)(ticks)(create_time))
CHAINBASE_SET_INDEX_TYPE(news::base::comment_vote_object, news::base::comment_vote_object_index)

