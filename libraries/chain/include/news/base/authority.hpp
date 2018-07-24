//
// Created by boy on 18-6-29.
//

#pragma once


#include <news/base/types.hpp>

namespace news{
    namespace base{

        typedef  int32_t weight_type;


        struct shared_authority{
            public_key_type         key;
            int32_t                 weight_type;
        };

    }
}


FC_REFLECT(news::base::shared_authority, (key)(weight_type))