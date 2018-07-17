//
// Created by boy on 18-7-16.
//

#pragma once

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <news/base/types.hpp>
#include <news/chain/transaction.hpp>

namespace news{
    namespace plugins{
        namespace account_history_api{



            using namespace news::base;



            struct operation_obj_api{
                operation_obj_api(){}
                template <typename  T>
                operation_obj_api(const T& obj)
                        :trx_id(obj.trx_id)
                        ,trx_in_block(obj.trx_in_block)
                        ,op_in_trx(obj.trx_in_block)
                        ,block(obj.block)
                        ,timestamp(obj.timestamp){
                    op = fc::raw::unpack_from_buffer<news::base::operation>(obj.op_packed);
                }

                news::base::transaction_id_type trx_id;
                uint32_t                    block = 0;
                uint32_t                    trx_in_block = 0;
                uint32_t                    op_in_trx = 0;
                fc::time_point_sec          timestamp;
                news::base::operation       op;

                bool operator<( const operation_obj_api& obj ) const
                {
                    return std::tie( block, trx_in_block, op_in_trx) < std::tie( obj.block, obj.trx_in_block, obj.op_in_trx);
                }
            };




            struct get_transaction_args{
                transaction_id_type     trx_id;
            };

            struct get_transaction_return{
                news::chain::deatil_signed_transaction trx;
            };


            struct get_account_history_args{
                account_name    name;
                uint32_t        start;
                uint32_t        limit;
            };


            struct get_account_history_return{
                std::map<uint32_t, operation_obj_api> trxs;
            };
        }
    }
}


FC_REFLECT(news::plugins::account_history_api::get_transaction_args, (trx_id))
FC_REFLECT(news::plugins::account_history_api::get_transaction_return, (trx))

FC_REFLECT(news::plugins::account_history_api::get_account_history_args, (name)(start)(limit))
FC_REFLECT(news::plugins::account_history_api::get_account_history_return, (trxs))
FC_REFLECT(news::plugins::account_history_api::operation_obj_api, (trx_id)(block)(trx_in_block)(op_in_trx)(timestamp)(op))




