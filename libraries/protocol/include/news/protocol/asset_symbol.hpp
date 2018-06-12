//
// Created by boy on 18-6-12.
//

#pragma once

namespace news{
    namespace protocol{

        struct asset_symbol{
            asset_symbol(){}

            bool operator == (const asset_symbol &a, const asset_symbol &b){
                //TODO
                return true;
            }
        };


    }//namespace protocol
}//namespace news



