#include <burst/integer/divceil.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("divceil")
{
    TEST_CASE("Возвращает результат деления, округлённый вверх до ближайшего целого")
    {
        CHECK(burst::divceil(13, 5) == 3);
        CHECK(burst::divceil(12u, 5u) == 3u);
    }

    TEST_CASE("Отрицательные числа округляются в сторону нуля")
    {
        CHECK(burst::divceil(-4, 3) == -1);
        CHECK(burst::divceil(-3, 3) == -1);
        CHECK(burst::divceil(-2, 3) == 0);
        CHECK(burst::divceil(-1, 3) == 0);
    }

    TEST_CASE("Функция burst::divceil для кратных чисел эквивалентна целочисленному делению")
    {
        CHECK(burst::divceil(36u, 6) == 6);
        CHECK(burst::divceil(25, 5) == 5);
        CHECK(burst::divceil(-16, 4u) == -4);
    }

    TEST_CASE("Результат деления нуля — ноль")
    {
        CHECK(burst::divceil(0, 1) == 0);
        CHECK(burst::divceil(0u, 100500) == 0);
    }

    TEST_CASE("Неопределена при неположительном делителе")
    {
        CHECK_THROWS_AS(burst::divceil(10u, 0ul), std::domain_error);
        CHECK_THROWS_AS(burst::divceil(10ul, -1), std::domain_error);
    }

    TEST_CASE("Деление на единицу равно исходному делимому")
    {
        CHECK(burst::divceil(15, 1) == 15);
        CHECK(burst::divceil(-3, 1) == -3);
    }

    TEST_CASE("Если делимое положительно, а делитель превышает делимое, то разультат равен единице")
    {
        CHECK(burst::divceil(5, 6) == 1);
    }

    TEST_CASE("Если делимое отрицательно, а делитель превышает делимое по модулю, то результат "
        "равен нулю")
    {
        CHECK(burst::divceil(-5, 6) == 0);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::divceil(21ul, 2ul);
        CHECK(x == 11);
    }

    TEST_CASE("Никогда не переполняется")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        constexpr auto int32_min = std::numeric_limits<std::int32_t>::min();
        constexpr auto uint32_max = std::numeric_limits<std::uint32_t>::max();

        CHECK(burst::divceil(uint32_max, uint32_max) == 1);
        CHECK(burst::divceil(uint32_max, 10) == 429496730);
        CHECK(burst::divceil(int32_max, int32_max) == 1);
        CHECK(burst::divceil(int32_max, uint32_max) == 1);
        CHECK(burst::divceil(int32_max, 10) == 214748365);
        CHECK(burst::divceil(0u, 10) == 0);
        CHECK(burst::divceil(0u, uint32_max) == 0);
        CHECK(burst::divceil(int32_min, 10) == -214748364);
        CHECK(burst::divceil(int32_min, uint32_max) == 0);
    }

    TEST_CASE("Допускает разные типы делимого и делителя")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        CHECK(burst::divceil(int32_max - 1, std::uint64_t{int32_max}) == 1);
        CHECK(burst::divceil(int32_max - 1, std::uint32_t{int32_max}) == 1);
        CHECK(burst::divceil(std::int16_t{-1}, std::uint32_t{256}) == 0);
        CHECK(burst::divceil(std::int32_t{-1}, std::uint16_t{256}) == 0);
        CHECK(burst::divceil(std::uint32_t{5}, std::int32_t{2}) == 3);
        CHECK(burst::divceil(std::uint32_t{5}, std::int64_t{2}) == 3);
        CHECK(burst::divceil(std::uint32_t{5}, std::int32_t{256}) == 1);
        CHECK(burst::divceil(std::uint32_t{5}, std::int64_t{256}) == 1);
        CHECK(burst::divceil(std::uint32_t{0}, std::int32_t{256}) == 0);
        CHECK(burst::divceil(std::uint32_t{0}, std::int64_t{256}) == 0);
        CHECK(burst::divceil(std::int8_t{0}, std::uint8_t{25}) == 0);
        CHECK(burst::divceil(std::uint16_t{5}, std::int16_t{25}) == 1);
    }

    TEST_CASE("Тип возвращаемого значения равен типу первого аргумента, то есть делимого")
    {
        using first_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int64_t{5}));
        CHECK(std::is_same<first_result_type, std::uint32_t>::value);
        using second_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int32_t{5}));
        CHECK(std::is_same<second_result_type, std::uint32_t>::value);
        using third_result_type = decltype(burst::divceil(std::uint32_t{2}, std::uint64_t{5}));
        CHECK(std::is_same<third_result_type, std::uint32_t>::value);
        using fourth_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int16_t{5}));
        CHECK(std::is_same<fourth_result_type, std::uint32_t>::value);
    }
}
