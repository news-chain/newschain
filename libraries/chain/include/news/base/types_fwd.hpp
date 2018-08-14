//
// Created by boy on 18-6-30.
//
#pragma once

namespace fc {
    class uint128;
    class variant;
} // fc

namespace news { namespace base {
        template< typename Storage = fc::uint128 >
        class fixed_string_impl;

//        class asset_symbol_type;
//        class legacy_ _asset_symbol_type;
//        struct legacy_ _asset;
    } } // news::base

namespace fc { namespace raw {

        template<typename Stream>
        inline void pack( Stream& s, const uint128& u );
        template<typename Stream>
        inline void unpack( Stream& s, uint128& u );

        template< typename Stream, typename Storage >
        inline void pack( Stream& s, const news::base::fixed_string_impl< Storage >& u );
        template< typename Stream, typename Storage >
        inline void unpack( Stream& s, news::base::fixed_string_impl< Storage >& u );
//
//        template< typename Stream >
//        inline void pack( Stream& s, const news::base::asset_symbol_type& sym );
//        template< typename Stream >
//        inline void unpack( Stream& s, news::base::asset_symbol_type& sym );
//
//        template< typename Stream >
//        inline void pack( Stream& s, const news::base::legacy_ _asset_symbol_type& sym );
//        template< typename Stream >
//        inline void unpack( Stream& s, news::base::legacy_ _asset_symbol_type& sym );

    } // raw

    template< typename Storage >
    inline void to_variant( const news::base::fixed_string_impl< Storage >& s, fc::variant& v );
    template< typename Storage >
    inline void from_variant( const variant& v, news::base::fixed_string_impl< Storage >& s );

//    inline void to_variant( const news::base::asset_symbol_type& sym, fc::variant& v );

//    inline void from_variant( const fc::variant& v, news::base::legacy_ _asset& leg );
//    inline void to_variant( const news::base::legacy_ _asset& leg, fc::variant& v );

} // fc
