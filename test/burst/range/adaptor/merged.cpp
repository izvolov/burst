#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/merged.hpp>
#include <burst/range/make_range_vector.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

TEST_SUITE("merged")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto first = burst::make_vector({1, 2, 3});
        const auto second = burst::make_vector({3, 4, 5});
        auto ranges = burst::make_range_vector(first, second);

        const auto merged = boost::make_iterator_range(ranges) | burst::merged;

        const auto expected = {1, 2, 3, 3, 4, 5};
        CHECK(merged == expected);
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        auto range_vector = burst::make_vector({boost::irange(1, 3), boost::irange(2, 4)});
        auto ranges = boost::make_iterator_range(range_vector);

        const auto merged = ranges | burst::merged;

        const auto expected = {1, 2, 2, 3};
        CHECK(merged == expected);
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        const auto first = burst::make_forward_list({3, 2, 1});
        const auto second = burst::make_forward_list({5, 3, 1});
        auto ranges = burst::make_range_vector(first, second);

        const auto merged =
            boost::make_iterator_range(ranges)
                | burst::merged([] (const auto & left, const auto & right) { return left > right; });

        const auto expected = {5, 3, 3, 2, 1, 1};
        CHECK(merged == expected);
    }

    TEST_CASE("Может работать с кортежем диапазонов")
    {
        const auto first = burst::make_vector({1, 4, 7});
        const auto second = burst::make_vector({2, 5, 8});
        const auto third = burst::make_forward_list({3, 6, 9});

        const auto merged = std::tie(first, second, third) | burst::merged;

        const auto expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(merged == expected);
    }
}
