#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/take_n.hpp>
#include <utility/io/forward_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("take_n")
{
    TEST_CASE("takes_exactly_specified_number_of_elements")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        const auto taken = burst::take_n(items, items_to_take);

        using std::begin;
        const auto expected =
            boost::make_iterator_range(begin(items), std::next(begin(items), items_to_take));
        CHECK(taken == expected);
    }

    TEST_CASE("pokerface_taking_too_many_elements")
    {
        const auto items = burst::make_vector({'a', 'b', 'c', 'd'});

        const auto items_to_take = 100500;
        const auto taken = burst::take_n(items, items_to_take);

        CHECK(std::distance(taken.begin(), taken.end()) == items_to_take);
    }

    TEST_CASE("taken_elements_of_mutable_range_can_be_modified")
    {
        auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        auto taken = burst::take_n(items, items_to_take);

        for (auto & x: taken)
        {
            x *= 10;
        }

        CHECK(items == burst::make_forward_list({10, 20, 30, 4}));
    }

    TEST_CASE("taking_random_access_range_produces_random_access_range")
    {
        const int items[] = {1, 2, 3};

        const auto items_to_take = 3;
        const auto taken = burst::take_n(items, items_to_take);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(std::begin(taken))>));
    }
}
