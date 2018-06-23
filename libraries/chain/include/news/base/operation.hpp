//
// Created by boy on 18-6-18.
//

#pragma once

#include <news/base/types.hpp>
#include <news/base/config.hpp>
#include <fc/static_variant.hpp>
#include <fc/reflect/reflect.hpp>
namespace news{
    namespace base{


       struct base_operation{
           virtual  const account_name& get_sign_name() const = 0;
           virtual  void validate() const = 0 ;
       };

        struct create_account_operation : public base_operation{
            account_name    name;
            account_name    creator;
            public_key_type public_key;
            void validate() const;
            const account_name& get_sign_name() const {return creator;}
        };


        struct transfer_operation : public base_operation{
            account_name    from;
            account_name    to;
            std::string     memo;
            void validate() const;
            const account_name& get_sign_name() const{return from;}
        };


        struct transfers_operation : public base_operation{
            account_name                    from;
            std::map<account_name, int32_t> to_names;
            std::string                     memo;
            void validate() const;
            const account_name& get_sign_name() const {return from;}
        };


        typedef fc::static_variant<create_account_operation,
                transfer_operation,
                transfers_operation>
                operation;



    }//news::base
}//news


FC_REFLECT_TYPENAME(news::base::operation)

FC_REFLECT(news::base::create_account_operation, (name)(creator)(public_key))
FC_REFLECT(news::base::transfer_operation, (from)(to)(memo))
FC_REFLECT(news::base::transfers_operation, (from)(to_names)(memo))

