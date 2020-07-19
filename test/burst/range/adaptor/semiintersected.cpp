#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/semiintersected.hpp>
#include <burst/range/make_range_vector.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <functional>

TEST_SUITE("semiintersected")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto  primes = burst::make_vector({   2, 3,    5, 7, 11});
        const auto natural = burst::make_vector({1, 2, 3, 4, 5       });
        //                                          ^  ^     ^
        auto ranges = burst::make_range_vector(primes, natural);

        const auto semiintersected = boost::make_iterator_range(ranges) | burst::semiintersected(2u);

        const auto expected = {2, 3, 5};
        CHECK(semiintersected == expected);
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        auto range_vector =
            burst::make_vector
            ({
                boost::irange(1,       4),
                boost::irange(   2,       5),
                boost::irange(      3,       6)
                //               ^  ^  ^
            });
        auto ranges = boost::make_iterator_range(range_vector);

        const auto semiintersected = ranges | burst::semiintersected(2u);

        const auto expected = {2, 3, 4};
        CHECK(semiintersected == expected);
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        const auto  first = burst::make_forward_list({   4, 3, 2, 1});
        const auto second = burst::make_forward_list({5,    3,    1});
        //                                                  ^     ^
        auto ranges = burst::make_range_vector(first, second);

        const auto semiintersected =
            boost::make_iterator_range(ranges)
                | burst::semiintersected(2u, std::greater<>{});

        const auto expected = {3, 1};
        CHECK(semiintersected == expected);
    }

    TEST_CASE("Может работать с кортежем диапазонов")
    {
        const auto  first = burst::make_vector      ({0, 0, 1, 1, 2, 2      });
        const auto second = burst::make_vector      ({0,    1,    2, 2      });
        const auto  third = burst::make_forward_list({      1,    2,    3, 4});
        //                                            ^     ^     ^  ^

        const auto semiintersected = std::tie(first, second, third) | burst::semiintersected(2);

        const auto expected = {0, 1, 2, 2};
        CHECK(semiintersected == expected);
    }
}
