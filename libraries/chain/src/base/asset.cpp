//
// Created by boy on 18-6-25.
//



#include <news/base/asset.hpp>
#include <news/base/asset_symbol.hpp>
namespace news{
    namespace base{

        asset asset::from_string(const std::string &str) {
            try {
                asset ret;
                std::string s = fc::trim(str);
                auto find_point = s.find(',');
                FC_ASSERT(find_point != std::string::npos, "Asset from_string , not find ','.");

                auto amount_str = fc::trim(s.substr(0, find_point));
                auto symbol_str = fc::trim(s.substr(find_point)).erase(0, 1);

                auto dot_pos = amount_str.find('.');
                FC_ASSERT(dot_pos != std::string::npos, "missing missing dot");

                std::string precision_digit_str = std::to_string(amount_str.size() - dot_pos - 1);

                symbol_str.erase(0, symbol_str.find_first_not_of(" "));
                ret.symbol = asset_symbol::from_string(symbol_str);

                int64_t int_part = 0;
                int64_t fract_part = 0;

                int_part = fc::to_int64(amount_str.substr(0, dot_pos));
                fract_part = fc::to_int64(amount_str.substr(dot_pos + 1));

                ret.amount = int_part * ret.precision();
                ret.amount += fract_part;

                return ret;
            }FC_CAPTURE_AND_RETHROW()
        }



        std::string asset::to_string() const {
            std::string ret = fc::to_string(static_cast<int64_t >(amount) / precision() );
            if(decimals()){
                auto fr = static_cast<int64_t >(amount) % precision();
                ret += "." + fc::to_string(precision() + fr).erase(0, 1);
            }

//                //remove 0
//            uint64_t pos = ret.length() + 1;
//            for(auto itr = ret.rbegin(); itr != ret.rend(); itr++){
//                if(*itr == '.'){
//                    pos += 3;
//                    break;
//                }else if(*itr != '0'){
//                    break;
//                }
//                else{
//                    pos--;
//                }
//            }
//            if(pos < ret.length()){
//                pos--;
//                ret.erase(pos, ret.length() - pos);
//            }
            return ret + ", " +symbol.name();
        }



        ///////////////////////asset_symbol////////////////////
        asset_symbol asset_symbol::from_string(const std::string &name) {
            FC_ASSERT(name == "NEWT" || name == " NEWT", "asset_symbol name != NEWT");
            asset_symbol sy(NEWS_SYMBOL);
            return sy;
        }
    }
}



namespace fc{
    void to_variant(const news::base::asset &a, fc::variant &v){
        try {
            v = a.to_string();
        }FC_CAPTURE_AND_RETHROW()
    }
    void from_variant(const fc::variant &v, news::base::asset &a){
        try {
            a = news::base::asset::from_string(v.get_string());
        }FC_CAPTURE_AND_RETHROW()
    }


//    void to_variant(const news::base::asset_symbol &a, fc::variant &v){
//        try{
//            v = mutable_variant_object("name", a.name())("precision", a.decimals());
//        }FC_CAPTURE_AND_RETHROW()
//    }
//
//    void from_variant(const fc::variant &v, news::base::asset_symbol &a){
//        try {
//            FC_ASSERT(v.is_object(), "asset_symbol is not object");
//
//            const auto &obj = v.get_object();
//
//            FC_ASSERT(obj.contains("name"), "asset_symbol symbol doesnt exit.");
//            FC_ASSERT(obj.contains("precision"), "asset_symbol precision doesnt exit.");
//
//            std::string name = boost::lexical_cast<std::string>(obj["name"].as<std::string>());
//            uint8_t p = obj["precision"].as<uint8_t>();
//
//            news::base::asset_symbol b(name, p);
//            a = b;
//
//        }FC_CAPTURE_AND_RETHROW()
//
//    }
}