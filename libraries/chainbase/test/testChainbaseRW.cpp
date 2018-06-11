//
// Created by boy on 18-5-30.
//

#define BOOST_TEST_MODULE chainbase test

#include <boost/test/unit_test.hpp>
#include <chainbase/chainbase.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <thread>

using namespace chainbase;
using namespace boost::multi_index;

//BOOST_TEST_SUITE( serialization_tests, clean_database_fixture )

struct book : public chainbase::object<0, book> {

    template<typename Constructor, typename Allocator>
    book(  Constructor&& c, Allocator&& a ){
        c(*this);
    }

    id_type     id;
    uint32_t    a;
    uint32_t         b;
};

typedef multi_index_container<
        book,
        indexed_by<
                ordered_unique< member<book,book::id_type,&book::id> >,
//                ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(book,shared_string,str1) >,
                ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(book,uint32_t,a) >,
                ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(book,uint32_t,b) >
        >,
        chainbase::allocator<book>
> book_index;

CHAINBASE_SET_INDEX_TYPE( book, book_index )

BOOST_AUTO_TEST_SUITE(open_and_create_rw)

    BOOST_AUTO_TEST_CASE( open_and_create_rw ) {
//        boost::filesystem::path temp = boost::filesystem::unique_path();
        boost::filesystem::path temp("1831-1fc3-5011-eb5f");
        try {
            std::cerr << temp.relative_path() << " \n";

            chainbase::database db;
//            BOOST_CHECK_THROW( db.open( temp), std::runtime_error ); /// temp does not exist
            db.open( temp, database::read_write, 1024*1024*1024 * 12L );
//            BOOST_CHECK_THROW( db.add_index<book_index>(), std::logic_error ); /// cannot add same index twice
            db.add_index<book_index>();

            BOOST_TEST_MESSAGE( "Creating book" );


            auto now = boost::posix_time::second_clock::local_time();

//            std::string str = "abcdef";

//            for(uint64_t j = 0; j < 10000000; j++){
//                db.create<book>([]( book& b ) {
//                    b.a = 1;
//                    b.b = 2;
//                } );
//            }


//
            for(uint64_t j = 0; j < 10000000; j++){
                const auto &bk = db.get(book::id_type(j));
                db.modify(bk, [](book &b){
                    b.a = 3;
                    b.b = 4;
                });



            }

            auto end = boost::posix_time::second_clock::local_time();
            auto spend = end.time_of_day().total_microseconds() - now.time_of_day().total_microseconds();
            std::cout << "end - now " << spend << std::endl;
//            while (true);

            db.close();


        } catch ( ... ) {
            bfs::remove_all( temp );
            throw;
        }
    }

BOOST_AUTO_TEST_SUITE_END()
