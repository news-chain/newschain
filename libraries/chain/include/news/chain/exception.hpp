//
// Created by boy on 18-6-14.
//

#pragma once


#include <fc/exception/exception.hpp>


#define NEWS_ASSERT( expr, exc_type, FORMAT, ... )                \
   FC_MULTILINE_MACRO_BEGIN                                           \
   if( !(expr) )                                                      \
      FC_THROW_EXCEPTION( exc_type, FORMAT, __VA_ARGS__ );            \
   FC_MULTILINE_MACRO_END



namespace news{
    namespace chain{
        FC_DECLARE_EXCEPTION( chain_exception, 4000000, "blockchain exception" )
        FC_DECLARE_DERIVED_EXCEPTION( unlinkable_block_exception, chain_exception, 4010000, "unlinkable block" );
    }
}
