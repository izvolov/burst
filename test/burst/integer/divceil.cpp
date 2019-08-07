#include <burst/integer/divceil.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("divceil")
{
    TEST_CASE("returns_ceiled_integral_part_of_division")
    {
        CHECK(burst::divceil(13, 5) == 3);
        CHECK(burst::divceil(12u, 5u) == 3u);
    }

    TEST_CASE("negative_numbers_rounds_toward_zero")
    {
        CHECK(burst::divceil(-4, 3) == -1);
        CHECK(burst::divceil(-3, 3) == -1);
        CHECK(burst::divceil(-2, 3) == 0);
        CHECK(burst::divceil(-1, 3) == 0);
    }

    TEST_CASE("divceil_of_a_multiple_is_equal_to_the_result_of_an_integral_division")
    {
        CHECK(burst::divceil(36u, 6) == 6);
        CHECK(burst::divceil(25, 5) == 5);
        CHECK(burst::divceil(-16, 4u) == -4);
    }

    TEST_CASE("divceil_of_0_is_always_0")
    {
        CHECK(burst::divceil(0, 1) == 0);
        CHECK(burst::divceil(0u, 100500) == 0);
    }

    TEST_CASE("undefined_on_non_positive_divisor")
    {
        CHECK_THROWS_AS(burst::divceil(10u, 0ul), std::domain_error);
        CHECK_THROWS_AS(burst::divceil(10ul, -1), std::domain_error);
    }

    TEST_CASE("divceil_of_a_number_by_1_is_the_same_number")
    {
        CHECK(burst::divceil(15, 1) == 15);
        CHECK(burst::divceil(-3, 1) == -3);
    }

    TEST_CASE("division_by_greater_in_magnitude_divisor_is_one_for_positive_dividend")
    {
        CHECK(burst::divceil(5, 6) == 1);
    }

    TEST_CASE("division_by_greater_in_magnitude_divisor_is_zero_for_non_positive_dividend")
    {
        CHECK(burst::divceil(-5, 6) == 0);
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto x = burst::divceil(21ul, 2ul);
        CHECK(x == 11);
    }

    TEST_CASE("does_not_overflow")
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

    TEST_CASE("accepts_different_argument_types")
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

    TEST_CASE("return_value_type_is_type_of_first_argument")
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
