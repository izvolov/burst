#include <burst/iterator/merge_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <functional>
#include <iterator>

TEST_SUITE("merge_iterator")
{
    TEST_CASE("merge_iterator_end_is_created_using_special_tag")
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = burst::make_range_vector(first, second);

        auto merged_begin = burst::make_merge_iterator(ranges, std::greater<>{});
        auto merged_end = burst::make_merge_iterator(burst::iterator::end_tag, merged_begin);

        auto expected_collection = {600, 500, 200, 100};
        CHECK(boost::make_iterator_range(merged_begin, merged_end) == expected_collection);
    }
}
