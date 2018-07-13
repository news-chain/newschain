//
// Created by boy on 18-7-3.
//


#pragma once


#include <news/base/types.hpp>
#include <news/base/operation.hpp>


namespace news{
    namespace base{
        struct operation_notification{
            operation_notification(const operation &o):op(o){

            }
            transaction_id_type     trx_id;
            uint32_t                block = 0;
            uint32_t                trx_in_block = 0;
            const operation         &op;
        };
    }
}



//FC_REFLECT(news::base::operation_notification, (trx_id)(block)(trx_in_block)(op))