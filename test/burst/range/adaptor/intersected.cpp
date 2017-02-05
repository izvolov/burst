#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/intersected.hpp>
#include <burst/range/make_range_vector.hpp>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>

BOOST_AUTO_TEST_SUITE(intersected)
    BOOST_AUTO_TEST_CASE(accepts_a_range_by_rvalue)
    {
        const auto  primes = burst::make_vector({   2, 3,    5, 7, 11});
        const auto natural = burst::make_vector({1, 2, 3, 4, 5       });
        //                                          ^  ^     ^
        auto ranges = burst::make_range_vector(primes, natural);

        const auto intersected = boost::make_iterator_range(ranges) | burst::intersected;

        const auto expected = {2, 3, 5};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected), std::end(intersected),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_a_range_by_lvalue)
    {
        auto range_vector = burst::make_vector({boost::irange(1, 4), boost::irange(2, 5)});
        auto ranges = boost::make_iterator_range(range_vector);

        const auto intersected = ranges | burst::intersected;

        const auto expected = {2, 3};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected), std::end(intersected),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_custom_comparator)
    {
        const auto  first = burst::make_forward_list({   4, 3, 2, 1});
        const auto second = burst::make_forward_list({5,    3,    1});
        //                                                  ^     ^
        auto ranges = burst::make_range_vector(first, second);

        const auto intersected =
            boost::make_iterator_range(ranges)
                | burst::intersected([] (const auto & left, const auto & right) { return left > right; });

        const auto expected = {3, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected), std::end(intersected),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
