#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/symmetric_difference.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <vector>

TEST_SUITE("symmetric_difference")
{
    TEST_CASE("Состоит из элементов, встречающихся в нечётном количестве входных диапазонов")
    {
        const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
        const auto     odd = burst::make_vector({1,    3,    5,    7,    9,     11    });
        const auto   prime = burst::make_vector({   2, 3,    5,    7,           11    });
        //                                             ^  ^  ^  ^  ^  ^     ^   ^   ^
        auto ranges = burst::make_range_vector(natural, odd, prime);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {3, 4, 5, 6, 7, 8, 10, 11, 12};
        CHECK(symmetric_difference == expected);
    }

    TEST_CASE("Симметрическая разность пустых диапазонов — пустой диапазон")
    {
        const auto first = std::vector<int>{};
        const auto second = std::vector<int>{};
        auto ranges = burst::make_range_vector(first, second);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        CHECK(symmetric_difference.empty());
    }

    TEST_CASE("Симметрическая разность одного диапазона — сам этот диапазон")
    {
        const auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        CHECK(symmetric_difference == boost::irange(1, 5));
    }

    TEST_CASE("Симметрическая разность нечётного количества равных диапазонов равна "
        "каждому из этих диапазонов")
    {
        const auto first = {4, 4, 5, 6, 6, 7};
        const auto second = first;
        const auto third = first;

        auto ranges = burst::make_range_vector(first, second, third);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        CHECK(symmetric_difference == first);
    }

    TEST_CASE("Симметрическая разность чётного количества равных диапазонов пуста")
    {
        const auto first = {4, 4, 5, 6, 6, 7};
        const auto second = first;
        const auto third = first;
        const auto fourth = first;

        auto ranges = burst::make_range_vector(first, second, third, fourth);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        CHECK(symmetric_difference.empty());
    }

    TEST_CASE("Симметрическая разность непересекающихся диапазонов эквивалентна из объединению")
    {
        const auto  first = burst::make_list({'a',           'd',           'g'     });
        const auto second = burst::make_list({     'b',           'e',           'h'});
        const auto  third = burst::make_list({          'c',           'f',         });
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
        CHECK(symmetric_difference == expected);
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        const auto  first = {   3, 3, 2, 1};
        const auto second = {4, 3,    2   };
        //                   ^     ^     ^
        auto ranges = burst::make_range_vector(first, second);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::greater<>{});

        const auto expected = {4, 3, 1};
        CHECK(symmetric_difference == expected);
    }

    TEST_CASE("Симметрическая разность последовательных диапазонов эквивалентна из объединению")
    {
        const auto  first = {1, 2, 3                  };
        const auto second = {         4, 5, 6         };
        const auto  third = {                  7, 8, 9};
        //                   ^  ^  ^  ^  ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::less<>{});

        const auto expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(symmetric_difference == expected);
    }

    TEST_CASE("Повторяющиеся элементы не порождают избыточных совпадений")
    {
        const auto  first = {1, 1, 1};
        const auto second = {1, 1, 1};
        const auto  third = {1, 1, 1};
        //                   ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::less<>{});

        CHECK(symmetric_difference == first);
    }

    TEST_CASE("Работает с диапазонами как с мультимножествами")
    {
        const auto  first = {0, 0, 1,       2};
        const auto second = {0,    1, 1      };
        const auto  third = {      1, 1, 1, 2};
        //                      ^  ^     ^

        auto ranges = burst::make_range_vector(first, second, third);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {0, 1, 1};
        CHECK(symmetric_difference == expected);
    }
}
