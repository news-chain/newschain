//
// Created by boy on 18-6-16.
//


#include <fc/reflect/reflect.hpp>
#include <fc/log/logger.hpp>
#include <fc/io/varint.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/io/json.hpp>

namespace test{
    struct testRef{
        int aaa = 0;
        int bbb = 0;
    };
}

FC_REFLECT(test::testRef, (aaa)(bbb))



int main(int argc, char **argv) {

    test::testRef tt;
    tt.aaa = 1000;
    tt.bbb = 2000;
    auto str = fc::json::to_string(tt);

    auto kk = fc::json::from_string(str).as<test::testRef>();

    edump((kk));


    return 0;
}