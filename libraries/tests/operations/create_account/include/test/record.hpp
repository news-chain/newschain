//
// Created by boy on 18-7-31.
//

#pragma once


#include <string>
#include <vector>

#include <news/chain/transaction.hpp>
#include <atomic>
#include <test/types.hpp>

#include <boost/signals2.hpp>

namespace  test{

    using namespace news::chain;
    using namespace news::base;


    typedef boost::signals2::signal<void(const tools::get_context &, bool success)>   resultSignal;
    typedef resultSignal::slot_type     slotType;

    class record{
    public:

        record();
        std::vector<std::string> add_record_data(std::vector<signed_transaction> &vec);

        void add_send_data(uint64_t id, tools::get_context cxt);
        bool update_data(uint64_t id, tools::get_context cxt);

        void log_data_and_move();
        void stop();

        boost::signals2::connection add_post_signal(slotType func);

    private:
        std::map<uint64_t, tools::get_context>  _data;
        std::atomic_long                    _id;
        resultSignal                        _post_result;
    };



}