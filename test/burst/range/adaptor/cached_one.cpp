#include <burst/range/adaptor/cached_one.hpp>

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cached_one)
    BOOST_AUTO_TEST_CASE(accepts_a_range_by_lvalue)
    {
        const auto range = boost::irange(0, 5);

        const auto cached = range | burst::cached_one;

        BOOST_CHECK(cached == range);
    }

    BOOST_AUTO_TEST_CASE(accepts_a_range_by_rvalue)
    {
        const auto cached = boost::irange(0, 5) | burst::cached_one;
        BOOST_CHECK(cached == boost::irange(0, 5));
    }
BOOST_AUTO_TEST_SUITE_END()
