//
// Created by boy on 18-6-19.
//

#pragma once

#include <string>
#include <fc/reflect/reflect.hpp>
#include <fc/variant.hpp>





namespace news{
    namespace base{

        /*
         * this use control version
         * v_num & 0xff000000 >> 24 = major
         * v_num & 0x00ff0000 >> 16 = minor
         * v_num & 0x0000ffff       = rev
         * */
       struct version{
            version(){}
            version(uint8_t m, uint8_t h, uint16_t r){
                v_num = ( 0 | m) << 8;
                v_num = ( v_num | h) << 16;
                v_num = ( v_num | r) << 24;
            }
            virtual ~version(){};
            bool operator == (const version &v)const{return v.v_num == v_num;}
            bool operator != (const version &v)const{return v.v_num != v_num;}
            bool operator < (const version &v)const {return v.v_num < v_num;}
            bool operator <= (const version &v)const {return v.v_num <= v_num;}
            bool operator > (const version &v)const {return v.v_num > v_num;}
            bool operator >= (const version &v)const {return v.v_num >= v_num;}

            operator std::string() const{
                std::stringstream ss;
                ss << ( ( v_num >> 24 ) & 0x000000FF )
                  << '.'
                  << ( ( v_num >> 16 ) & 0x000000FF )
                  << '.'
                  << ( ( v_num & 0x0000FFFF ) );

                return ss.str();
            };

            uint32_t v_num = 1;
       };






    }//news::base
}//news

namespace fc{
    void to_variant(const news::base::version &v, fc::variant &var);
}


FC_REFLECT(news::base::version, (v_num))
