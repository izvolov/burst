#include <iterator>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/union_iterator.hpp>

BOOST_AUTO_TEST_SUITE(unite_iterator)
    BOOST_AUTO_TEST_CASE(unite_iterator_end_is_created_using_special_tag)
    {
        auto  first = {1, 2, 2, 3};
        auto second = {0, 2, 2, 4};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto union_begin = burst::make_union_iterator(ranges);
        auto   union_end = burst::make_union_iterator(ranges, burst::iterator::end_tag);

        auto expected_collection = {0, 1, 2, 2, 3, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            union_begin, union_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
