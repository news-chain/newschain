//
// Created by boy on 18-6-13.
//

#pragma once





namespace news{
    namespace chain{


        class block_summary_object;



        enum object_type{
            global_property_dynamic_obj = 0,
            block_summary_object_type,
            transaction_object_type,
            account_object_type,
            account_authority_type,
            operation_object_type,
            account_history_object_type,
            hardfork_property_object_type
        };

//        typedef oid<block_summary_object>           block_summary_object_id_type;



    }// news::chain
}//news
