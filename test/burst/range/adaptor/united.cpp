#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/united.hpp>
#include <burst/range/make_range_vector.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

TEST_SUITE("united")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto  primes = burst::make_vector({   2, 3,    5, 7, 11});
        const auto natural = burst::make_vector({1, 2, 3, 4, 5       });
        auto ranges = burst::make_range_vector(primes, natural);

        const auto united = boost::make_iterator_range(ranges) | burst::united;

        const auto expected = {1, 2, 3, 4, 5, 7, 11};
        CHECK(united == expected);
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        auto range_vector = burst::make_vector({boost::irange(1, 4), boost::irange(2, 5)});
        auto ranges = boost::make_iterator_range(range_vector);

        const auto united = ranges | burst::united;

        const auto expected = {1, 2, 3, 4};
        CHECK(united == expected);
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        const auto  first = burst::make_forward_list({   4, 3, 2, 1});
        const auto second = burst::make_forward_list({5,    3,    1});
        auto ranges = burst::make_range_vector(first, second);

        const auto united =
            boost::make_iterator_range(ranges)
                | burst::united([] (const auto & left, const auto & right) { return left > right; });

        const auto expected = {5, 4, 3, 2, 1};
        CHECK(united == expected);
    }

    TEST_CASE("Может работать с кортежем диапазонов")
    {
        const auto  first = burst::make_vector      ({0, 0, 1, 1, 2, 2      });
        const auto second = burst::make_vector      ({0,    1,    2, 2      });
        const auto  third = burst::make_forward_list({      1,    2,    3, 4});

        const auto united = std::tie(first, second, third) | burst::united;

        const auto expected = {0, 0, 1, 1, 2, 2, 3, 4};
        CHECK(united == expected);
    }
}
