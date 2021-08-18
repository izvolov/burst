#include <burst/string/u8s.hpp>

#include <doctest/doctest.h>

#include <ostream>
#include <string>
#include <type_traits>

TEST_SUITE("u8s")
{
    TEST_CASE("На выходе — строка, содержащая указанные символы")
    {
        using burst::literals::operator ""_u8s;

        const auto result = "this is a string"_u8s;
        CHECK(result == std::string("this is a string"));

        const auto u8result = u8"это строка"_u8s;
        const auto u8expected =
            "\xd1\x8d\xd1\x82\xd0\xbe\x20\xd1\x81\xd1\x82\xd1\x80\xd0\xbe\xd0\xba\xd0\xb0";
        CHECK(u8result == u8expected);
    }

    TEST_CASE("Тип результата — всегда std::string")
    {
        using burst::literals::operator ""_u8s;

        auto result = "this is a string"_u8s;
        CHECK(std::is_same<decltype(result), std::string>::value);

        auto u8result = u8"это строка"_u8s;
        CHECK(std::is_same<decltype(u8result), std::string>::value);
    }
}
