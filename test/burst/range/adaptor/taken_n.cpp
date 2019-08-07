#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/taken_n.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("taken_n")
{
    TEST_CASE("takes_exactly_specified_number_of_elements")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3l;
        const auto taken = items | burst::taken_n(items_to_take);

        using std::begin;
        const auto expected =
            boost::make_iterator_range(begin(items), std::next(begin(items), items_to_take));

        CHECK(taken == expected);
    }

    TEST_CASE("accepts_a_range_by_rvalue")
    {
        const auto taken = boost::irange(0, 100500) | burst::taken_n(5l);

        CHECK(taken == boost::irange(0, 5));
    }
}
