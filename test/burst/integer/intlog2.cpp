#include <burst/integer/intlog2.hpp>

#include <doctest/doctest.h>

#include <stdexcept>

TEST_SUITE("intlog2")
{
    TEST_CASE("throws_beyond_the_domain")
    {
        CHECK_THROWS_AS(burst::intlog2(0ul), std::domain_error);
    }

    TEST_CASE("intlog2_of_one_is_zero")
    {
        CHECK(burst::intlog2(1u) == 0);
    }

    TEST_CASE("intlog2_of_power_of_two_is_exactly_that_power")
    {
        CHECK(burst::intlog2(8ul) == 3);
    }

    TEST_CASE("intlog2_of_non_power_of_two_is_integer_part_of_logarithm")
    {
        CHECK(burst::intlog2(15ul) == 3);
    }

    TEST_CASE("intlog2_is_a_constexpr_function")
    {
        constexpr auto x = burst::intlog2(100ul);
        CHECK(x == 6);
    }

    TEST_CASE("accepts_signed_values")
    {
        CHECK(burst::intlog2(signed{5}) == 2);
    }
}
