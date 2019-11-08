#include <burst/iterator/intersect_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("intersect_iterator")
{
    TEST_CASE("Конец итератора пересечения создаётся из его начала с помощью специальной "
        "метки-индикатора")
    {
        auto  first = {1, 2, 3};
        auto second = {0, 2, 4};
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_begin =
            burst::make_intersect_iterator(ranges);
        auto intersected_end =
            burst::make_intersect_iterator(burst::iterator::end_tag, intersected_begin);

        auto expected = {2};
        CHECK(boost::make_iterator_range(intersected_begin, intersected_end) == expected);
    }
}
