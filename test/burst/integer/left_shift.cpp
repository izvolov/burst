#include <burst/integer/left_shift.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <type_traits>

TEST_SUITE("left_shift")
{
    TEST_CASE("Тип возвращаемого значения равен типу первого аргумента, "
        "то есть сдвигаемого значения")
    {
        {
            using result_type = decltype(burst::left_shift(std::int8_t{2}, std::int64_t{5}));
            CHECK(std::is_same<result_type, std::int8_t>::value);
        }
        {
            using result_type = decltype(burst::left_shift(std::uint16_t{2}, std::uint8_t{5}));
            CHECK(std::is_same<result_type, std::uint16_t>::value);
        }
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto value = burst::left_shift(0b101, 1);
        CHECK(value == 0b1010);
    }
}
