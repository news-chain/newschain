//
// Created by boy on 18-6-19.
//






#include <news/base/operation.hpp>



namespace news{
    namespace base{

        bool validate_account_name(const account_name &n){
            return true;
        }



        void create_account_operation::validate() const {
            validate_account_name(name);
            validate_account_name(creator);
        }

        void transfer_operation::validate() const {
            validate_account_name(from);
            validate_account_name(to);
            FC_ASSERT(from != to, "dont allow transfer to yourself.");
            FC_ASSERT(amount.amount > 0, "transfer amount must > 0.");
        }

        void transfers_operation::validate() const{
            validate_account_name(from);
            for(const auto &b : this->to_names){
                validate_account_name(b.first);
                FC_ASSERT(from != b.first, "dont allow tranfer to yourself.");
                FC_ASSERT(b.second.amount > 0, "transfer balance must > 0.");
            }
        }


    }//news::base
}//news