#include <burst/algorithm/sum.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sum")
{
    TEST_CASE("sum_is_a_constexpr_function")
    {
        constexpr auto x = burst::sum({1.62, 2.72, 3.14});
        CHECK(x == doctest::Approx(7.48).epsilon(0.0001));
    }

    TEST_CASE("sum_of_one_element_equals_that_element")
    {
        CHECK(burst::sum({-1}) == -1);
    }

    TEST_CASE("sum_of_empty_list_is_zero")
    {
        auto empty = std::initializer_list<std::size_t>{};
        CHECK(burst::sum(empty) == 0ul);
    }

    TEST_CASE("accepts_variadic")
    {
        constexpr auto x = burst::sum(1, 2, 3);
        CHECK(x == 6);
    }
}
