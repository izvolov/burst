#include <burst/container/make_deque.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/intersect.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <string>
#include <vector>

TEST_SUITE("intersect")
{
    TEST_CASE("Пересечение пустых диапазонов — пустой диапазон")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        CHECK(intersected_range.empty());
    }

    TEST_CASE("Пересечение одного диапазона — сам этот диапазон")
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto intersected_range = burst::intersect(ranges);

        CHECK(intersected_range == boost::irange(1, 5));
    }

    TEST_CASE("Пересечение одинаковых диапазонов — диапазон, равный всем исходным")
    {
        auto first = {4, 5, 6, 7};
        auto second = first;
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        CHECK(intersected_range == first);
    }

    TEST_CASE("Пересечение вложенных диапазонов равно наименьшему из них")
    {
        std::string  long_range("abcdef");
        std::string short_range("cde");
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto intersected_range = burst::intersect(ranges);

        CHECK(intersected_range == short_range);
    }

    TEST_CASE("Пересечение пилообразно дополняющих друг друга диапазонов пусто")
    {
        auto  first = burst::make_list({'h',      'f',      'd',      'b'     });
        auto second = burst::make_list({     'g',      'e',      'c',      'a'});
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges, std::greater<>{});

        CHECK(intersected_range.empty());
    }

    TEST_CASE("Пересечение накладывающихся друг на друга диапазонов состоит из элементов "
        "этого наложения")
    {
        auto  first = {   3, 2, 1};
        auto second = {4, 3, 2   };
        //                ^  ^
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges, std::greater<>{});

        auto expected_collection = {3, 2};
        CHECK(intersected_range == expected_collection);
    }

    TEST_CASE("Пересечение диапазонов, накладывающихся попарно, но не совместно — пустой диапазон")
    {
        auto  first = {1, 2      };
        auto second = {   2, 3   };
        auto  third = {      3, 4};
        auto ranges = burst::make_range_vector(first, second, third);

        auto intersected_range = burst::intersect(ranges);

        CHECK(intersected_range.empty());
    }

    TEST_CASE("Пересечение нескольких последовательно дополняющих друг друга диапазонов — "
        "пустой диапазон")
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};

        auto intersected_range = burst::intersect(std::tie(first, second, third));

        CHECK(intersected_range.empty());
    }

    TEST_CASE("Работает с диапазонами как с мультимножествами")
    {
        auto  first = {1, 1, 1};
        auto second = {1, 1, 1};
        auto  third = {1, 1, 1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto intersected_range = burst::intersect(ranges, std::less<>{});

        CHECK(intersected_range == first);
    }

    TEST_CASE("Элементы не попадают в пересечение дважды")
    {
        auto  first = {0, 0, 1};
        auto second = {0, 1, 1};
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        auto expected_collection = {0, 1};
        CHECK(intersected_range == expected_collection);
    }

    TEST_CASE("Пересечение можно вызывать без предварительного создания диапазона диапазонов")
    {
        const auto  first = burst::make_list({2, 2, 1});
        const auto second = burst::make_list({2, 1, 1});

        auto intersected_range = burst::intersect(std::tie(first, second), std::greater<>{});

        auto expected_collection = {2, 1};
        CHECK(intersected_range == expected_collection);
    }

    TEST_CASE("Можно пересекать диапазоны разных типов")
    {
        auto   one = burst::make_vector({1, 2, 3, 4, 5, 6, 7       });
        auto   two = burst::make_list  ({1,    3,    5,    7, 9    });
        auto three = burst::make_deque ({1,       4,       7,    10});
        //                               ^                 ^

        auto intersected_range = burst::intersect(std::tie(one, two, three));

        auto expected_collection = {1, 7};
        CHECK(intersected_range == expected_collection);
    }

    TEST_CASE("Можно пересекать с предикатом диапазоны разных типов")
    {
        auto   one = burst::make_vector({      7, 6, 5, 4, 3, 2, 1, 0});
        auto   two = burst::make_list  ({   8,    6,    4,    2,    0});
        auto three = burst::make_deque ({9,       6,       3,       0});
        //                                        ^                 ^

        auto intersected_range = burst::intersect(std::tie(one, two, three), std::greater<>{});

        auto expected_collection = {6, 0};
        CHECK(intersected_range == expected_collection);
    }
}
