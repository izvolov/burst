#include <burst/integer/intceil.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("intceil")
{
    TEST_CASE("Возвращает наименьшее число, которое не меньше первого аргумента, "
        "и при этом кратно второму аргументу")
    {
        CHECK(burst::intceil(13, 5) == 15);
        CHECK(burst::intceil(12u, 5u) == 15u);
    }

    TEST_CASE("Возвращает само число, если оно изначально кратно второму аргументу")
    {
        CHECK(burst::intceil(36u, 6) == 36);
        CHECK(burst::intceil(25, 5) == 25);
    }

    TEST_CASE("burst::intceil от нуля — ноль")
    {
        CHECK(burst::intceil(0, 1) == 0);
        CHECK(burst::intceil(0, 100500) == 0);
    }

    TEST_CASE("Отрицательные числа округляются в сторону нуля")
    {
        CHECK(burst::intceil(-4, 3) == -3);
        CHECK(burst::intceil(-3, 3) == -3);
        CHECK(burst::intceil(-2, 3) == 0);
        CHECK(burst::intceil(-1, 3) == 0);
    }

    TEST_CASE("burst::intceil числа по показателю 1 — само это число")
    {
        CHECK(burst::intceil(15, 1) == 15);
        CHECK(burst::intceil(-3, 1) == -3);
    }

    TEST_CASE("Порождает исключение на неположительном показателе")
    {
        CHECK_THROWS_AS(burst::intceil(10u, 0ul), std::domain_error);
        CHECK_THROWS_AS(burst::intceil(10ul, -1), std::domain_error);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::intceil(21ul, 2ul);
        CHECK(x == 22);
    }

    TEST_CASE("Допускает разные типы первого и второго аргументов")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        CHECK(burst::intceil(int32_max - 1, std::uint64_t{int32_max}) == int32_max);
    }

    TEST_CASE("Тип возвращаемого значения равен типу первого аргумента")
    {
        using first_argument_type = std::uint32_t;
        using second_argument_type = std::int64_t;
        static_assert(not std::is_same<first_argument_type, second_argument_type>::value, "");

        using result_type = decltype(burst::intceil(first_argument_type{2}, second_argument_type{5}));

        CHECK(std::is_same<result_type, first_argument_type>::value);
    }
}
