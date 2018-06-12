//
// Created by boy on 18-6-12.
//

#pragma once


//#include <cstdint>
#include <fc/time.hpp>

namespace news{
    namespace chain{


        struct transaction{
            uint16_t                ref_block_num = 0;
            uint32_t                ref_block_prefix = 0;
            fc::time_point_sec      expiration;
        };

    }//namespace chain
}//namespace news