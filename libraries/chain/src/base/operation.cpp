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

		void comment_operation::validate() const {
			validate_account_name(author); 
			FC_ASSERT(title.length()< NEWS_COMMENT_TITLE_MAX_LEN, "title must greater ${t}", ("t",NEWS_COMMENT_TITLE_MAX_LEN)); 
		}
		void comment_vote_operation::validate() const {
			validate_account_name(voter);
			int count =0- NEWS_COMMENT_TITLES_MAX;  
			FC_ASSERT(ticks < count || ticks > NEWS_COMMENT_TITLES_MAX, "ticks must range ${t} ${t1}", ("t", NEWS_COMMENT_TITLE_MAX_LEN)("t", count));
		}
		 
		
        void packed_block_reward_operation::validate() const {
            FC_ASSERT(to_name == NEWS_SYSTEM_ACCEPT_NAME, "only system account.");
        }


    }//news::base
}//news