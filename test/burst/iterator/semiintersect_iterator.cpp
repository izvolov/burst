#include <burst/iterator/semiintersect_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("semiintersect_iterator")
{
    TEST_CASE("semiintersect_iterator_end_is_created_using_special_tag")
    {
        auto  first = {1, 2, 3};
        auto second = {0, 2, 4};
        auto ranges = burst::make_range_vector(first, second);

        auto min_items_to_score = 2ul;
        auto semiintersected_begin =
            burst::make_semiintersect_iterator(ranges, min_items_to_score);
        auto semiintersected_end =
            burst::make_semiintersect_iterator(burst::iterator::end_tag, semiintersected_begin);

        auto expected = {2};
        CHECK(boost::make_iterator_range(semiintersected_begin, semiintersected_end) == expected);
    }
}
