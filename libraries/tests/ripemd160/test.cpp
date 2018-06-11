//
// Created by boy on 18-6-10.
//


#define BOOST_TEST_MODULE RIPEMD160


#include <boost/test/unit_test_suite.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <iostream>
#include <fc/reflect/reflect.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/io/json.hpp>
#include <chainbase/chainbase.hpp>




using namespace chainbase;
using namespace boost::multi_index;





struct operation_test : public chainbase::object<0, operation_test>{

    operation_test() = delete;
public:
    template<typename Constructor, typename Allocator>
    operation_test(  Constructor&& c, Allocator&& a ):memo(a),trx_id(a) {
        c(*this);
    }

    id_type         id;
    uint64_t        from;
    uint64_t        to;
    shared_string   memo;
    shared_string   trx_id;
};


struct transfer{
    uint64_t from;
    uint64_t to;
    std::string memo;
};

struct by_trx_id;
struct by_id;

typedef boost::multi_index_container<
        operation_test,
        indexed_by<
                ordered_unique<member<operation_test, operation_test::id_type, &operation_test::id>
                        >
                ,ordered_unique< tag<by_trx_id>,
                    composite_key< operation_test,
                        member<operation_test, operation_test::id_type, &operation_test::id>,
                        member<operation_test, shared_string, &operation_test::trx_id>
                    >
                >
        >,
        chainbase::allocator<operation_test>
> operation_index;

CHAINBASE_SET_INDEX_TYPE( operation_test, operation_index )

FC_REFLECT(operation_test, (id)(from)(to)(memo)(trx_id));
FC_REFLECT(transfer, (from)(to)(memo));



BOOST_AUTO_TEST_SUITE(ripemd160_test)

    BOOST_AUTO_TEST_CASE(ripemd160_test){
        boost::filesystem::path temp = boost::filesystem::unique_path();
        try {
            chainbase::database db;
            db.open(temp, database::read_write, 1024 * 1024 * 1024 * 20L);
            db.add_index<operation_index>();

            std::string memo = "qwertyuiopasdfghjklzxcvbnm1234567890qwertyuiopasdfghjklzxcvbnm";



//            uint64_t i = 0;
            auto now = boost::posix_time::second_clock::local_time();
//            uint64_t  i = 0;
            for(uint64_t i = 0; i < 10000000; i++){
                db.create<operation_test>([&](operation_test &obj){
                    obj.from = i;
                    obj.to = i;
                    obj.memo.assign(memo.begin(), memo.end());

                    std::string ss = memo + std::to_string(i) + std::to_string(i);
//                fc::ripemd160 rp = fc::ripemd160::;
                    auto ret = fc::ripemd160::hash(ss).str();
                    obj.trx_id.assign(ret.begin(), ret.end());

                });
            }

            auto end = boost::posix_time::second_clock::local_time();
            auto spend = end.time_of_day().total_microseconds() - now.time_of_day().total_microseconds();
            std::cout << "end - now " << spend << std::endl;

            db.close();
        }catch (fc::exception &e){
            std::cout << "exception fc" << e.what() << std::endl;
        }
        catch (std::exception &e){
            std::cout << "std exception " << e.what() << std::endl;
        }
        catch (...){
            std::cout << "unhanle exception " << std::endl;
        }


    }




BOOST_AUTO_TEST_SUITE_END()




/*
 * 测试数据，10000000  33s
 *
 *
 *
 * */