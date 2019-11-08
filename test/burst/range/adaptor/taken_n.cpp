#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/taken_n.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("taken_n")
{
    TEST_CASE("Создаёт диапазон из префикса ровно заданной длины исходного диапазона")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3l;
        const auto taken = items | burst::taken_n(items_to_take);

        using std::begin;
        const auto expected =
            boost::make_iterator_range(begin(items), std::next(begin(items), items_to_take));

        CHECK(taken == expected);
    }

    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto taken = boost::irange(0, 100500) | burst::taken_n(5l);

        CHECK(taken == boost::irange(0, 5));
    }
}
