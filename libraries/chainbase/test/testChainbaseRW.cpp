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
#include <boost/multi_index/composite_key.hpp>

using namespace chainbase;
using namespace boost::multi_index;

//BOOST_TEST_SUITE( serialization_tests, clean_database_fixture )

struct book : public chainbase::object<0, book> {

    template<typename Constructor, typename Allocator>
    book(Constructor &&c, chainbase::allocator<Allocator> a) {
        c(*this);
    }

    id_type id;
    int a;
    int b;
};

struct by_name;
struct by_id;
struct by_ab;
typedef multi_index_container<
        book,
        indexed_by<
                ordered_unique<tag<by_id>,
                        member<book, book::id_type, &book::id>
                >

                , ordered_unique<tag<by_ab>,
                        composite_key<book,
                                member<book, int, &book::a>,
                                member<book, int, &book::b>
                        >,
                        composite_key_compare<std::less<int>, std::greater<int> >
                >
        >,
        chainbase::allocator<book>
>
        book_index;

CHAINBASE_SET_INDEX_TYPE(book, book_index)

BOOST_AUTO_TEST_SUITE(open_and_create_rw)

    BOOST_AUTO_TEST_CASE(open_and_create_rw) {
        boost::filesystem::path temp = boost::filesystem::unique_path();
//        boost::filesystem::path temp("1831-1fc3-5011-eb5f");
        try {
            std::cerr << temp.relative_path() << " \n";

            chainbase::database db;
//            BOOST_CHECK_THROW( db.open( temp), std::runtime_error ); /// temp does not exist
            db.open(temp, 0, 1024 * 1024 * 1024 * 12L);
//            BOOST_CHECK_THROW( db.add_index<book_index>(), std::logic_error ); /// cannot add same index twice
            db.add_index<book_index>();

            BOOST_TEST_MESSAGE("Creating book");
            auto now = boost::posix_time::second_clock::local_time();


            for(int i = 0; i < 10; i++){
                for(int j = 0; j < 10; j++){
                    db.create<book>([&](book &obj){
                        obj.a = i;
                        obj.b = j;
                    });
                }
            }

            const auto &itrx = db.get_index<book_index>().indices().get<by_ab>();
            auto ee = itrx.lower_bound(boost::make_tuple(3, 3));
            auto bb = itrx.upper_bound(boost::make_tuple(7, 7));
            while(bb != ee && bb != itrx.end()){
                std::cout << "id: " << bb->id._id << " a: " << bb->a << " b:" << bb->b << std::endl;
                bb++;
            }






        } catch (...) {
            bfs::remove_all(temp);
            throw;
        }
    }

BOOST_AUTO_TEST_SUITE_END()
