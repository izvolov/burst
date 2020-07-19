#include <burst/container/make_deque.hpp>
#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/symmetric_differenced.hpp>
#include <burst/range/make_range_vector.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <functional>

TEST_SUITE("symmetric_differenced")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
        const auto     odd = burst::make_vector({1,    3,    5,    7,    9,     11    });
        const auto   prime = burst::make_vector({   2, 3,    5,    7,           11    });
        //                                             ^  ^  ^  ^  ^  ^     ^   ^   ^
        auto ranges = burst::make_range_vector(natural, odd, prime);

        const auto symmetric_differenced = boost::make_iterator_range(ranges) | burst::symmetric_differenced;

        const auto expected = {3, 4, 5, 6, 7, 8, 10, 11, 12};
        CHECK(symmetric_differenced == expected);
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        auto range_vector =
            burst::make_vector
            ({
                boost::irange(1,       4),
                boost::irange(   2,       5),
                boost::irange(      3,       6)
                //            ^     ^     ^
            });
        auto ranges = boost::make_iterator_range(range_vector);

        const auto symmetric_differenced = ranges | burst::symmetric_differenced;

        const auto expected = {1, 3, 5};
        CHECK(symmetric_differenced == expected);
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        const auto  first = burst::make_forward_list({      4, 3, 2, 1});
        const auto second = burst::make_forward_list({5, 5,    3, 2, 1});
        const auto  third = burst::make_forward_list({5,    4, 3,    1});
        const auto fourth = burst::make_forward_list({      4,    2, 1});
        //                                               ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third, fourth);

        const auto symmetric_differenced =
            boost::make_iterator_range(ranges)
                | burst::symmetric_differenced(std::greater<>{});

        const auto expected = {5, 4, 3, 2};
        CHECK(symmetric_differenced == expected);
    }

    TEST_CASE("Может работать с кортежем диапазонов")
    {
        const auto  first = burst::make_forward_list({      4, 3, 2, 1});
        const auto second = burst::make_list        ({5, 5,    3, 2, 1});
        const auto  third = burst::make_vector      ({5,    4, 3,    1});
        const auto fourth = burst::make_deque       ({      4,    2, 1});
        //                                               ^  ^  ^  ^

        const auto symmetric_differenced =
            std::tie(first, second, third, fourth)
                | burst::symmetric_differenced(std::greater<>{});

        const auto expected = {5, 4, 3, 2};
        CHECK(symmetric_differenced == expected);
    }
}
