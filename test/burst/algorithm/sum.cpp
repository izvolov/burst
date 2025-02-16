#include <burst/algorithm/sum.hpp>

#include <doctest/doctest.h>

#include <cstddef>

TEST_SUITE("sum")
{
    TEST_CASE("Функция может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::sum({1.62, 2.72, 3.14});
        CHECK(x == doctest::Approx(7.48).epsilon(0.0001));
    }

    TEST_CASE("Сумма одного элемента равна этому элементу")
    {
        CHECK(burst::sum({-1}) == -1);
    }

    TEST_CASE("Сумма пустого списка равна нулю")
    {
        auto empty = std::initializer_list<std::size_t>{};
        CHECK(burst::sum(empty) == 0ul);
    }

    TEST_CASE("Допускает произвольное количество входных аргументов")
    {
        constexpr auto x = burst::sum(1, 2, 3);
        CHECK(x == 6);
    }
}
