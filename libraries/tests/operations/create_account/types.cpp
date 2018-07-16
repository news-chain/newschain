//
// Created by boy on 18-7-16.
//

#include "types.hpp"


namespace tools{


    bool result_has_error(const result_body &body){
        return body.error.valid();
    }



}