#include <burst/iterator/union_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <forward_list>
#include <iterator>

TEST_SUITE("union_iterator")
{
    TEST_CASE("Конец итератора объединения создаётся из его начала с помощью специальной "
        "метки-индикатора")
    {
        auto  first = {1, 2, 2, 3};
        auto second = {0, 2, 2, 4};
        auto ranges = burst::make_range_vector(first, second);

        auto union_begin = burst::make_union_iterator(ranges);
        auto   union_end = burst::make_union_iterator(burst::iterator::end_tag, union_begin);

        auto expected_collection = {0, 1, 2, 2, 3, 4};
        CHECK(boost::make_iterator_range(union_begin, union_end) == expected_collection);
    }

    TEST_CASE("Допускает однонаправленные диапазоны")
    {
        std::forward_list<double> forward_range{1.6, 2.71, 3.14};
        auto ranges = burst::make_range_vector(forward_range);

        auto union_begin = burst::make_union_iterator(ranges);
        auto   union_end = burst::make_union_iterator(burst::iterator::end_tag, union_begin);

        auto expected_collection = {1.6, 2.71, 3.14};
        CHECK(boost::make_iterator_range(union_begin, union_end) == expected_collection);
    }
}
