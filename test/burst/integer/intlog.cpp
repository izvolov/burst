#include <burst/integer/intlog.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("intlog")
{
    TEST_CASE("throws_beyond_the_domain")
    {
        CHECK_THROWS_AS(burst::intlog(0ul, 10ul), std::domain_error);
    }

    TEST_CASE("throws_on_non_positive_base")
    {
        CHECK_THROWS_AS(burst::intlog(10ul, 0ul), std::domain_error);
    }

    TEST_CASE("throws_on_base_equal_to_one")
    {
        CHECK_THROWS_AS(burst::intlog(10ul, 1ul), std::domain_error);
    }

    TEST_CASE("any_base_intlog_of_one_is_zero")
    {
        CHECK(burst::intlog(1u, 100500u) == 0);
    }

    TEST_CASE("intlog_of_power_of_its_base_is_exactly_that_power")
    {
        CHECK(burst::intlog(100ul, 10ul) == 2);
    }

    TEST_CASE("intlog_of_non_power_of_its_base_is_integer_part_of_logarithm")
    {
        CHECK(burst::intlog(30ul, 3ul) == 3);
    }

    TEST_CASE("intlog_is_a_constexpr_function")
    {
        constexpr auto x = burst::intlog(20ul, 2ul);
        CHECK(x == 4);
    }

    TEST_CASE("accepts_signed_values")
    {
        CHECK(burst::intlog(13, 3) == 2);
    }

    TEST_CASE("accepts_different_argument_types")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        CHECK(burst::intlog(std::int32_t{14}, std::uint64_t{int32_max} + 1) == 0);

        constexpr auto int8_max = std::numeric_limits<std::int8_t>::max();
        CHECK(burst::intlog(std::uint32_t{int8_max} + 1, std::int8_t{5}) == 3);
    }

    TEST_CASE("return_value_type_is_type_of_first_argument")
    {
        using first_argument_type = std::uint32_t;
        using second_argument_type = std::int64_t;
        static_assert(not std::is_same<first_argument_type, second_argument_type>::value, "");

        using result_type = decltype(burst::intlog(first_argument_type{2}, second_argument_type{5}));

        CHECK(std::is_same<result_type, first_argument_type>::value);
    }
}
