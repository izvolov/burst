#include <burst/functional/low_byte.hpp>

#include <doctest/doctest.h>

TEST_SUITE("low_byte")
{
    TEST_CASE("Может быть вычислен на этапе компиляции")
    {
        constexpr auto x = burst::low_byte(0xff01);
        static_assert(x == 0x01, "");
    }
}
