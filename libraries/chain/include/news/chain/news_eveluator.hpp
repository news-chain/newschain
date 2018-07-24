//
// Created by boy on 18-6-19.
//

#pragma once

#include <news/chain/evaluator.hpp>
#include <news/base/operation.hpp>

namespace news{
    namespace chain{

        using namespace news::base;


NEWS_DEFINE_EVALUATOR(create_account)
NEWS_DEFINE_EVALUATOR(transfer)
NEWS_DEFINE_EVALUATOR(transfers)



    }//news::base
}//news