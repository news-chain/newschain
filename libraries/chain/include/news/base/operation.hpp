//
// Created by boy on 18-6-18.
//

#pragma once

#include <news/base/types.hpp>
#include <news/base/asset.hpp>
#include <news/base/config.hpp>

#include <fc/static_variant.hpp>
#include <fc/reflect/reflect.hpp>
namespace news{
    namespace base{


       struct base_operation{
           virtual void get_sign_name(account_name &name) const = 0;
           virtual void validate() const = 0;
           bool is_virtual = false;
       };

        struct create_account_operation : public base_operation{
            account_name    name;
            account_name    creator;
            public_key_type public_key;
            void validate() const;
            void get_sign_name(account_name &name) const { name = creator; }
        };


        struct transfer_operation : public base_operation{ //zhuan zhang 
            account_name    from;
            account_name    to;
            asset           amount;
            std::string     memo;
            void validate() const;
            void get_sign_name(account_name &name) const { name = from; }
        };


        struct transfers_operation : public base_operation{ //pi liang zhuan zhang
            account_name                    from;
            std::map<account_name, asset>   to_names;
            std::string                     memo;
            void validate() const;
            void get_sign_name(account_name &name) const { name = from; }
        };


        typedef fc::static_variant<create_account_operation,
                transfer_operation,
                transfers_operation>
                operation;


        /********************************************************************
         *                      virtual operation
         ********************************************************************
         * */


        struct packed_block_reward : public base_operation{
            account_name            producer;
            asset                   reward;

        };



    }//news::base
}//news


FC_REFLECT_TYPENAME(news::base::operation)

FC_REFLECT(news::base::create_account_operation, (name)(creator)(public_key))
FC_REFLECT(news::base::transfer_operation, (from)(to)(amount)(memo))
FC_REFLECT(news::base::transfers_operation, (from)(to_names)(memo))

/********************************************************************
   *                      virtual operation
   ********************************************************************
   * */

FC_REFLECT(news::base::packed_block_reward, (producer)(reward))


