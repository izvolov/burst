#include <burst/integer/bitwise_not.hpp>

#include <doctest/doctest.h>

#include <type_traits>

TEST_SUITE("bitwise_not")
{
    TEST_CASE_TEMPLATE("Тип возвращаемого результата совпадает с исходным типом", integer_type,
        char, signed char, unsigned char, short, unsigned short, int, unsigned, long, unsigned long,
        long long, unsigned long long)
    {
        CHECK(std::is_same<decltype(burst::bitwise_not(integer_type{1})), integer_type>::value);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto r = burst::bitwise_not(-1);
        static_assert(r == ~-1, "");
    }
}
