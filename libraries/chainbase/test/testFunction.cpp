#define BOOST_TEST_MODULE chainbase test

#include <boost/test/unit_test.hpp>
#include <chainbase/chainbase.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>

using namespace chainbase;
using namespace boost::multi_index;

//BOOST_TEST_SUITE( serialization_tests, clean_database_fixture )

struct book : public chainbase::object<0, book> {

    template<typename Constructor, typename Allocator>
    book(  Constructor&& c, Allocator&& a ) {
        c(*this);
    }

    id_type id;
    int a = 0;
    int b = 1;
};

typedef multi_index_container<
        book,
        indexed_by<
                ordered_unique< member<book,book::id_type,&book::id> >,
                ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(book,int,a) >,
                ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(book,int,b) >
        >,
        chainbase::allocator<book>
> book_index;

CHAINBASE_SET_INDEX_TYPE( book, book_index )

BOOST_AUTO_TEST_SUITE(open_and_create)

    BOOST_AUTO_TEST_CASE( open_and_create ) {
        boost::filesystem::path temp = boost::filesystem::unique_path();
        try {
            std::cerr << temp.native() << " \n";

            chainbase::database db;
            db.open( "c461-c887-c4c1-87a9", 1, 1024*1024*8L);
            db.add_index< book_index >();



            db.undo_all();
            std::cout << "rev: " << db.revision() << std::endl;
            auto session = db.start_undo_session();

            db.create<book>([](book &b){
                std::cout << b.id._id << std::endl;
                b.a = 1;
                b.b = 2;
            });
            session.push();
//            db.commit(db.revision());

            std::cout << "rev: " << db.revision() << std::endl;






        } catch ( ... ) {
            bfs::remove_all( temp );
            throw;
        }
    }

BOOST_AUTO_TEST_SUITE_END()
