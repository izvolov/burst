#include <burst/functional/low_byte.hpp>

#include <doctest/doctest.h>

TEST_SUITE("low_byte")
{
    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto x = burst::low_byte(0xff01);
        static_assert(x == 0x01, "");
    }
}
