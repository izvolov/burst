#include <burst/container/make_deque.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/semiintersect.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <list>
#include <string>
#include <vector>

TEST_SUITE("semiintersect")
{
    TEST_CASE("Полупересечение пустых диапазонов — пустой диапазон")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection.empty());
    }

    TEST_CASE("Полупересечение с параметром, большим чем количество входных диапазонов — "
        "пустой диапазон")
    {
        auto  first = burst::make_vector({1, 2, 3, 4});
        auto second = burst::make_vector({1, 2, 3, 4});
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 10);

        CHECK(semiintersection.empty());
    }

    TEST_CASE("Полупересечение одного диапазона - сам этот диапазон")
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection == boost::irange(1, 5));
    }

    TEST_CASE("Полупересечение нескольких одинаковых диапазонов с параметром от единицы до "
        "количества входных диапазонов равно каждому из этих входных диапазонов")
    {
        auto first = {4, 4, 5, 6, 6, 7};
        auto second = first;
        auto third = first;

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection1 = burst::semiintersect(ranges, 1);

            CHECK(semiintersection1 == first);
        }

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection2 = burst::semiintersect(ranges, 2);

            CHECK(semiintersection2 == second);
        }

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection3 = burst::semiintersect(ranges, 3);

            CHECK(semiintersection3 == third);
        }
    }

    TEST_CASE("Полупересечение двух вложенных диапазонов с параметром 2 равно кратчайшему из них")
    {
        std::string  long_range("abcdef");
        std::string short_range(  "cde" );
        //                         ^^^
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto semiintersection = burst::semiintersect(ranges, 2);

        CHECK(semiintersection == short_range);
    }

    TEST_CASE("Полупересечение двух вложенных диапазонов с параметром 1 равно длиннейшему из них")
    {
        std::string short_range(   "cde" );
        std::string  long_range("aabcdef");
        //                       ^^^^^^^
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection == long_range);
    }

    TEST_CASE("Полупересечение пилообразно дополняющих друг друга диапазонов с параметром 2 пусто")
    {
        std::list<char>  first{'h',      'f',      'd',      'b'     };
        std::list<char> second{     'g',      'e',      'c',      'a'};
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 2, std::greater<>{});

        CHECK(semiintersection.empty());
    }

    TEST_CASE("Полупересечение пилообразно дополняющих друг друга диапазонов с параметром 1 "
        "эквивалентно их объединению")
    {
        std::list<char>  first{'h',      'f',      'd',      'b'     };
        std::list<char> second{     'g',      'e',      'c',      'a'};
        //                      ^    ^    ^    ^    ^    ^    ^    ^
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 1, std::greater<>{});

        CHECK(semiintersection == std::string("hgfedcba"));
    }

    TEST_CASE("Допускает пользовательскую функцию для сравнения элементов")
    {
        auto  first = {   3, 3, 2, 1};
        auto second = {4, 3,    2   };
        //                ^     ^
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 2, std::greater<>{});

        auto expected_collection = {3, 2};
        CHECK(semiintersection == expected_collection);
    }

    TEST_CASE("Полупересечение накладывающихся диапазонов с параметром 1 эквивалентно их "
        "объединению")
    {
        auto  first = {1, 2         };
        auto second = {   2, 3, 3   };
        auto  third = {      3, 3, 4};
        //             ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 1);

        auto expected = {1, 2, 3, 3, 4};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("Полупересечение накладывающихся диапазонов с параметром 2 состоит из элементов, "
        "попадающих в наложение")
    {
        auto  first = {1, 2      };
        auto second = {   2, 3   };
        auto  third = {      3, 4};
        //                ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 2);

        auto expected = {2, 3};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("Полупересечение нескольких последовательных диапазонов с параметром больше единицы "
        "пусто")
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 2, std::less<>{});

        CHECK(semiintersection.empty());
    }

    TEST_CASE("Полупересечение нескольких последовательных диапазонов с параметром, равным "
        "единице, эквивалентно их слиянию")
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};
        //             ^  ^  ^  ^  ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 1, std::less<>{});

        auto expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("Повторяющиеся элементы не порождают излишних совпадений")
    {
        auto  first = {1, 1, 1};
        auto second = {1, 1, 1};
        auto  third = {1, 1, 1};
        //             ^  ^  ^

        auto semiintersection =
            burst::semiintersect(std::tie(first, second, third), 2, std::less<>{});

        CHECK(semiintersection == first);
    }

    TEST_CASE("Работает с диапазонами как с мультимножествами")
    {
        auto  first = {0, 0, 1,       2};
        auto second = {0,    1, 1      };
        auto  third = {      1, 1, 1, 2};
        //             ^     ^  ^     ^

        auto ranges = burst::make_range_vector(first, second, third);
        auto semiintersection = burst::semiintersect(ranges, 2);

        auto expected_collection = {0, 1, 1, 2};
        CHECK(semiintersection == expected_collection);
    }

    TEST_CASE("Полупересечение можно вызывать без предварительного создания диапазона диапазонов")
    {
        const auto  first = {0, 0, 1,       2};
        const auto second = {0,    1, 1,    2};
        const auto  third = {0,    1, 1, 1, 2};
        //                   ^     ^        ^

        auto semiintersection = burst::semiintersect(std::tie(first, second, third), 3);

        const auto expected_collection = {0, 1, 2};
        CHECK(semiintersection == expected_collection);
    }

    TEST_CASE("Возможно полупересечение диапазонов разных типов")
    {
        auto   one = burst::make_vector({1, 2, 3, 4, 5, 6, 7       });
        auto   two = burst::make_list  ({1,    3,    5,    7, 9    });
        auto three = burst::make_deque ({1,       4,       7,    10});
        //                               ^     ^  ^  ^     ^

        auto semiintersection = burst::semiintersect(std::tie(one, two, three), 2);

        auto expected_collection = {1, 3, 4, 5, 7};
        CHECK(semiintersection == expected_collection);
    }

    TEST_CASE("Возможно полупересечение с предикатом диапазонов разных типов")
    {
        auto   one = burst::make_vector({      7, 6, 5, 4, 3, 2, 1, 0});
        auto   two = burst::make_list  ({   8,    6,    4,    2,    0});
        auto three = burst::make_deque ({9,       6,       3,       0});
        //                                        ^                 ^

        auto semiintersection =
            burst::semiintersect(std::tie(one, two, three), 3, std::greater<>{});

        auto expected_collection = {6, 0};
        CHECK(semiintersection == expected_collection);
    }
}
