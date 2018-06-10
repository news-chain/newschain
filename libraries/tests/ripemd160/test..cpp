//
// Created by boy on 18-6-10.
//


#define BOOST_TEST_MODULE RIPEMD160


#include <boost/test/unit_test_suite.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <iostream>
#include <fc/reflect/reflect.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/io/json.hpp>
#include <chainbase/chainbase.hpp>




using namespace chainbase;
using namespace boost::multi_index;





struct operation : public chainbase::object<0, operation>{

    template<typename Constructor, typename Allocator>
    operation(  Constructor&& c, Allocator&& a ):memo(a) {
        c(*this);
    }

    id_type         id;
    uint64_t        from;
    uint64_t        to;
    shared_string   memo;
//    fc::ripemd160   trx_id;
};



//typedef oid<>

struct by_trx_id;
struct by_id;

typedef boost::multi_index_container<
        operation,
        indexed_by<
                ordered_unique<member<operation,operation::id_type,&operation::id>
                        >
//                ordered_unique<
//                    composite_key< tag<by_trx_id>,
//                        member<operation, operation::id_type, &operation::id>
////                        member<operation, fc::ripemd160, &operation::trx_id>
//                    >
//                >
        >,
        chainbase::allocator<operation>
> operation_index;



//FC_REFLECT(operation, (id)(from)(to)(memo));



BOOST_AUTO_TEST_SUITE(ripemd160_test)

    BOOST_AUTO_TEST_CASE(ripemd160_test){
        boost::filesystem::path temp = boost::filesystem::unique_path();
        try {
            chainbase::database db;
            db.open(temp, database::read_write, 1024 * 1024 * 8);
            db.add_index<operation_index>();

//            std::string memo = "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm";
//            uint64_t i = 0;
//            db.create<operation>([&](operation &obj){
//                obj.from = i;
//                obj.to = i;
//                obj.memo.assign(memo.begin(), memo.end());
//
//                std::string str = fc::json::to_string(obj);
//                fc::ripemd160 ripemd160(str);
//
//                obj.trx_id = ripemd160;
//            });
        }catch (...){
        }


    }




BOOST_AUTO_TEST_SUITE_END()