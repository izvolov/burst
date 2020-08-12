#include <burst/algorithm/copy_at_most_n.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <deque>
#include <iterator>
#include <list>
#include <vector>

TEST_SUITE("copy_at_most_n")
{
    TEST_CASE("Копирует n элементов, если размер диапазона не меньше n")
    {
        const auto initial = std::vector<int>{9, 8, 7, 6};

        auto result = std::vector<int>();
        burst::copy_at_most_n(initial, 2, std::back_inserter(result));

        CHECK(result == std::vector<int>{9, 8});
    }

    TEST_CASE("Копирует весь диапазон, если его размер меньше n")
    {
        const auto initial = std::vector<int>{1, 2, 3, 4};

        auto result = std::vector<int>(initial.size());
        burst::copy_at_most_n(initial, 100500, result.begin());

        CHECK(result == initial);
    }

    TEST_CASE("Возвращает итератор за последним скопированным элментом исходного диапазона")
    {
        const auto initial = std::vector<unsigned>{4, 5, 6, 7};
        const auto n = 3;

        auto result = std::vector<unsigned>();
        auto triple = burst::copy_at_most_n(initial, n, std::back_inserter(result));

        CHECK(std::get<0>(triple) == std::next(initial.begin(), n));
    }

    TEST_CASE("Возвращает конец исходного диапазона, если элементов не хватило")
    {
        const auto initial = std::list<long>{9, 8, 7, 6};

        auto result = std::list<long>();
        auto triple = burst::copy_at_most_n(initial, 100500, std::back_inserter(result));

        CHECK(std::get<0>(triple) == initial.end());
    }

    TEST_CASE("Возвращает количество скопированных элементов")
    {
        const auto initial = std::deque<char>{'a', 'b', 'c'};
        const auto n = 2u;

        auto result = std::deque<int>();
        auto triple = burst::copy_at_most_n(initial, n, std::back_inserter(result));

        CHECK(std::get<1>(triple) == n);
    }

    TEST_CASE("Количество скопированных элементов равно размеру диапазона, если размер диапазона "
        "меньше n")
    {
        const auto initial = std::deque<char>{'a', 'b', 'c', 'd'};

        auto result = std::deque<int>();
        auto triple = burst::copy_at_most_n(initial, 100500, std::back_inserter(result));

        CHECK(std::get<1>(triple) == initial.size());
    }

    TEST_CASE("Возвращает итератор за последним скопированным элементом в результирующем диапазоне")
    {
        const auto initial = {'a', 'b', 'c', 'd', 'e'};
        const auto n = 4ul;

        auto result = std::vector<char>(initial.size());
        auto triple = burst::copy_at_most_n(initial, n, result.begin());

        CHECK(std::get<2>(triple) == std::next(result.begin(), n));
    }

    TEST_CASE("Умеет принимать список инициализации")
    {
        const auto n = 3l;

        auto result = std::vector<char>();
        burst::copy_at_most_n({'q', 'w', 'e', 'r', 't', 'y'}, n, std::back_inserter(result));

        CHECK(result == std::vector<char>{'q', 'w', 'e'});
    }
}
