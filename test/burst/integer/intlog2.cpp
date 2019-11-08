#include <burst/integer/intlog2.hpp>

#include <doctest/doctest.h>

#include <stdexcept>

TEST_SUITE("intlog2")
{
    TEST_CASE("Порождает исключение, если логарифмируемое число находится вне области определения")
    {
        CHECK_THROWS_AS(burst::intlog2(0ul), std::domain_error);
    }

    TEST_CASE("Логарифм единицы по основанию 2 — ноль")
    {
        CHECK(burst::intlog2(1u) == 0);
    }

    TEST_CASE("Логарифм степени двойки в точности равен этой степени")
    {
        CHECK(burst::intlog2(8ul) == 3);
    }

    TEST_CASE("Возвращает целую часть логарифма числа по основанию 2")
    {
        CHECK(burst::intlog2(15ul) == 3);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::intlog2(100ul);
        CHECK(x == 6);
    }

    TEST_CASE("Допускает аргумент знакового типа")
    {
        CHECK(burst::intlog2(signed{5}) == 2);
    }
}
