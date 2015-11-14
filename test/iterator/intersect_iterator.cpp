#include <iterator>

#include <boost/test/unit_test.hpp>

#include <burst/iterator/intersect_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

BOOST_AUTO_TEST_SUITE(intersect_iterator)
    BOOST_AUTO_TEST_CASE(intersect_iterator_end_is_created_using_special_tag)
    {
        auto  first = {1, 2, 3};
        auto second = {0, 2, 4};
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_begin = burst::make_intersect_iterator(ranges);
        auto   intersected_end = burst::make_intersect_iterator(ranges, burst::iterator::end_tag);

        auto expected_collection = {2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            intersected_begin, intersected_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
