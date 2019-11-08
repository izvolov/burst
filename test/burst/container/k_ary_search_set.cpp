#include <burst/container/k_ary_search_set.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

namespace burst
{
    template <typename Value, typename Compare, typename Collection>
    bool operator == (const k_ary_search_set<Value, Compare> & set, const Collection & c)
    {
        return
            std::mismatch
            (
                set.begin(), set.end(),
                c.begin(), c.end()
            ) == std::make_pair(set.end(), c.end());
    }
} // namespace burst

TEST_SUITE("k_ary_search")
{
    TEST_CASE("k-местное дерево, проинициализированное конструктором по умолчанию, пусто")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.empty());
    }

    TEST_CASE("Размер k-местного дерева, проинициализированного конструктором по умолчанию, "
        "равен нулю")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.size() == 0);
    }

    TEST_CASE("Начало k-местного дерева, проинициализированного конструктором по умолчанию, "
        "равно его началу")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.begin() == set.end());
    }

    TEST_CASE("В k-местном дереве, проинициализированном конструктором по умолчанию, "
        "ничего нельзя найти")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.find("qwerty") == set.end());
    }

    TEST_CASE("k-местное дерево, проинициализированное пустым диапазоном, пусто")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.empty());
    }

    TEST_CASE("Размер k-местного дерева, проинициализированного пустым диапазоном, равен нулю")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.size() == 0);
    }

    TEST_CASE("Размер k-местного дерева равен количеству элементов диапазона, из которых оно "
        "проиницилизировано, в случае, если известно, что в этом диапазоне нет повторяющихся "
        "элементов")
    {
        auto something = burst::make_vector({1, 2, 3, 4});

        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, something.begin(), something.end());

        CHECK(set.size() == something.size());
    }

    TEST_CASE("k-местное дерево поиска представляет собой совершенное дерево при количестве "
        "элементов, равном `(k - 1) * sum_{i = 1}^{n}(k ^ (i - 1))`, где `n` — глубина "
        "дерева")
    {
        auto numbers = burst::make_vector({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        auto expected = {7, 10, 2, 5, 8, 9, 11, 12, 0, 1, 3, 4, 6};
        CHECK(set == expected);
    }

    TEST_CASE("k-местное дерево поиска представляет собой завершённое (complete) дерево в случае, "
        "если элементов недостаточно для того, чтобы оно было совершенным")
    {
        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, {0, 1, 2, 3, 4, 5, 6, 7}, 3);

        auto expected = {2, 5, 0, 1, 3, 4, 6, 7};
        CHECK(set == expected);
    }

    TEST_CASE("Местность дерева может быть больше его размера")
    {
        auto numbers = burst::make_vector({0, 1, 2, 3, 4, 5, 6, 7});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 129);

        CHECK(set == numbers);
    }

    TEST_CASE("В k-местном дереве, проинициализированном пустым диапазоном, нельзя ничего найти")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.find(26) == set.end());
    }

    TEST_CASE("Поиск заведомо несуществующего элемента возвращает итератор на конец")
    {
        burst::k_ary_search_set<int> set({3, 4, 6, 1, 7, 8, 2}, 2);

        CHECK(set.find(26) == set.end());
    }

    TEST_CASE("Поиск заведомо существующего значения возвращает итератор не на конец")
    {
        auto numbers = burst::make_vector({3, 4, 6, 1, 7, 8, 2});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        CHECK(set.find(8) != set.end());
    }

    TEST_CASE("Поиск заведомо существующего значения возвращает итератор на искомое значение")
    {
        auto numbers = burst::make_vector({3, 4, 6, 1, 7, 8, 2});

        burst::k_ary_search_set<int, std::greater<>> set(numbers.begin(), numbers.end(), 4);

        CHECK(*set.find(3) == 3);
    }

    TEST_CASE("Нет ошибки округления при логарифмировании")
    {
        std::vector<int> numbers(1000);
        std::iota(numbers.rbegin(), numbers.rend(), 0);

        burst::k_ary_search_set<int, std::greater<>> set(numbers.begin(), numbers.end(), 10);

        CHECK(*set.find(50) == 50);
    }

    TEST_CASE("У метода find() есть константный вариант")
    {
        const burst::k_ary_search_set<int> set;
        CHECK(set.find(8) == set.end());
    }

    TEST_CASE("У метода find() есть неконстантный вариант")
    {
        burst::k_ary_search_set<int> set;
        burst::k_ary_search_set<int> & set_ref = set;
        CHECK(set_ref.find(8) == set_ref.end());
    }
}
