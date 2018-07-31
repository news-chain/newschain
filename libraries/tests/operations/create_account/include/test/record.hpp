//
// Created by boy on 18-7-31.
//

#pragma once


#include <string>
#include <vector>

#include <news/chain/transaction.hpp>
#include <atomic>
#include <test/types.hpp>
namespace  test{

    using namespace news::chain;
    using namespace news::base;

    class record{
    public:

        record();
        std::vector<std::string> add_record_data(std::vector<signed_transaction> &vec);

        void add_send_data(uint64_t id, tools::get_context cxt);
        bool update_data(uint64_t id, tools::get_context cxt);

        void log_data_and_move();
    private:
        std::map<uint64_t, tools::get_context>  _data;
//        uint64_t                            _id;
        std::atomic_long                    _id;
    };



}