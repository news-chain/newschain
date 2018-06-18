//
// Created by boy on 18-6-18.
//

#pragma once

#include <news/base/types.hpp>
#include <chainbase/chainbase.hpp>
#include <news/chain/object_types.hpp>


namespace news{
    namespace base{

    class account_object : public chainbase::object<account_object_type, account_object>{
    public:
        CHAINBASE_DEFAULT_CONSTRUCTOR(account_object)

        id_type             id;
        account_name        name;
        uint64_t            balance = 0;
       fc::time_point       create_time;
    };


    }//news::base
}//news


FC_REFLECT(news::base::account_object, (id)(name)(balance)(create_time))

