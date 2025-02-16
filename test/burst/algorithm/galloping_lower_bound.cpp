#include <burst/algorithm/galloping_lower_bound.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <ostream>
#include <string>
#include <vector>

TEST_SUITE("galloping_lower_bound")
{
    TEST_CASE("Поиск в пустом диапазоне возвращает конец этого диапазона")
    {
        std::vector<int> empty;

        auto search_result = burst::galloping_lower_bound(empty.begin(), empty.end(), 1);

        CHECK(search_result == empty.end());
    }

    TEST_CASE("Поиск элемента, большего всех элементов данного диапазона, возвращает конец этого "
        "диапазона")
    {
        std::vector<std::size_t> range{1, 2, 3, 4, 5};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 100500ul);

        CHECK(search_result == range.end());
    }

    TEST_CASE("Поиск элемента, меньшего всех элементов данного диапазона, возвращает начало этого "
        "диапазона")
    {
        std::list<std::int8_t> range{10, 20, 30, 40};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 0);

        CHECK(search_result == range.begin());
    }

    TEST_CASE("Поиск отсутствующего в диапазоне элемента возвращает итератор на первый элемент "
        "диапазона, больший искомого элемента")
    {
        auto range = {50, 40, 30, 20, 10};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 33, std::greater<>{});

        CHECK(search_result == range.begin() + 2);
        CHECK(*search_result == 30);
    }

    TEST_CASE("Поиск существующего в диапазоне элемента возвращает итератор на этот элемент")
    {
        std::vector<std::string> range{"abc", "bc", "c"};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), std::string("bc"));

        CHECK(search_result == range.begin() + 1);
        CHECK(*search_result == "bc");
    }

    TEST_CASE("Поиск существующего в диапазоне элемента возвращает итератор на первый из "
        "повторяющихся элементов")
    {
        std::vector<int> range{1, 1, 1, 2, 2, 2, 3, 3, 3};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 3);

        CHECK(search_result == range.begin() + 6);
        CHECK(*search_result == 3);
    }

    TEST_CASE("Поиск несуществующего в диапазоне элемента возвращает итератор на первый из "
        "повторяющихся элементов")
    {
        std::vector<int> range{30, 30, 30, 20, 20, 20, 10, 10, 10};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 25, std::greater<>{});

        CHECK(search_result == range.begin() + 3);
        CHECK(*search_result == 20);
    }

    TEST_CASE("Элементы диапазона должны быть разделены относительно искомого значения")
    {
        auto range = {3, 2, 1, 4, 7, 6, 5};
        //                        |
        //                        |  |
        //                        |  |  |
        //                     |  |  |  |
        //            |        |  |  |  |
        //            |  |     |  |  |  |
        //            |  |  |  |  |  |  |
        //                     ^

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 4);
        CHECK(search_result == range.begin() + 3);
        CHECK(*search_result == 4);
    }

    TEST_CASE("Допускает диапазоны")
    {
        const auto range = {1, 2, 3, 4, 5};

        const auto search_result = burst::galloping_lower_bound(range, 3);

        CHECK(search_result == range.begin() + 2);
        CHECK(*search_result == 3);
    }
}
