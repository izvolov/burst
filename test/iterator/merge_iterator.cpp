#include <functional>
#include <iterator>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/merge_iterator.hpp>

BOOST_AUTO_TEST_SUITE(merge_iterator)
    BOOST_AUTO_TEST_CASE(merge_iterator_end_is_created_using_special_tag)
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_begin = burst::make_merge_iterator(ranges, std::greater<int>());
        auto   merged_end = burst::make_merge_iterator(ranges, std::greater<int>(), burst::iterator::end_tag);

        auto expected_collection = {600, 500, 200, 100};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            merged_begin, merged_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
