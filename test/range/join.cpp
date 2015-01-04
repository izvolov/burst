#include <string>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/range/join.hpp>

BOOST_AUTO_TEST_SUITE(join)
    BOOST_AUTO_TEST_CASE(join_function_accepts_inplace_initializer_list)
    {
        std::string hello("hello");
        std::string world("world");

        auto joint_range = burst::join
        ({
            boost::make_iterator_range(hello),
            boost::make_iterator_range(world)
        });

        std::string helloworld("helloworld");
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            helloworld.begin(), helloworld.end(),
            joint_range.begin(), joint_range.end()
        );
    }
BOOST_AUTO_TEST_SUITE_END()
