#include <burst/integer/intceil.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("intceil")
{
    TEST_CASE("returns_first_number_not_less_than_initial_which_is_a_multiple_of_factor")
    {
        CHECK(burst::intceil(13, 5) == 15);
        CHECK(burst::intceil(12u, 5u) == 15u);
    }

    TEST_CASE("intceil_of_number_which_is_already_a_multiple_of_factor_is_itself")
    {
        CHECK(burst::intceil(36u, 6) == 36);
        CHECK(burst::intceil(25, 5) == 25);
    }

    TEST_CASE("intceil_of_0_is_always_0")
    {
        CHECK(burst::intceil(0, 1) == 0);
        CHECK(burst::intceil(0, 100500) == 0);
    }

    TEST_CASE("negative_numbers_rounds_toward_zero")
    {
        CHECK(burst::intceil(-4, 3) == -3);
        CHECK(burst::intceil(-3, 3) == -3);
        CHECK(burst::intceil(-2, 3) == 0);
        CHECK(burst::intceil(-1, 3) == 0);
    }

    TEST_CASE("intceil_of_a_number_by_1_is_the_same_number")
    {
        CHECK(burst::intceil(15, 1) == 15);
        CHECK(burst::intceil(-3, 1) == -3);
    }

    TEST_CASE("throws_on_non_positive_factor")
    {
        CHECK_THROWS_AS(burst::intceil(10u, 0ul), std::domain_error);
        CHECK_THROWS_AS(burst::intceil(10ul, -1), std::domain_error);
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto x = burst::intceil(21ul, 2ul);
        CHECK(x == 22);
    }

    TEST_CASE("accepts_different_argument_types")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        CHECK(burst::intceil(int32_max - 1, std::uint64_t{int32_max}) == int32_max);
    }

    TEST_CASE("return_value_type_is_type_of_first_argument")
    {
        using first_argument_type = std::uint32_t;
        using second_argument_type = std::int64_t;
        static_assert(not std::is_same<first_argument_type, second_argument_type>::value, "");

        using result_type = decltype(burst::intceil(first_argument_type{2}, second_argument_type{5}));

        CHECK(std::is_same<result_type, first_argument_type>::value);
    }
}
