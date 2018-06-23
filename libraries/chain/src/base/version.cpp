//
// Created by boy on 18-6-19.
//

#include <news/base/version.hpp>

namespace fc{
    void to_variant(const news::base::version &v, fc::variant &var){
        var = std::string(v);
    }
}