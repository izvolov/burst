#include <burst/bit/bit_cast.hpp>

#include <doctest/doctest.h>

#include <cstdint>

TEST_SUITE("bit_cast")
{
    TEST_CASE("Преобразует тип значения, сохраняя его битовое представление")
    {
        //                                                   знак степень  мантисса
        //                                                      v v        v
        CHECK(burst::bit_cast<std::uint32_t>(3.14f)        == 0b0'10000000'10010001111010111000011);
        CHECK(burst::bit_cast<std::uint32_t>(-3.14f)       == 0b1'10000000'10010001111010111000011);
        CHECK(burst::bit_cast<std::uint32_t>(1.0f / 0.0f)  == 0b0'11111111'00000000000000000000000);
        CHECK(burst::bit_cast<std::uint32_t>(-1.0f / 0.0f) == 0b1'11111111'00000000000000000000000);
    }

#if defined(__cpp_lib_bit_cast)
    TEST_CASE("Функция может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::bit_cast<std::uint64_t>(-0.0);
        static_assert(x == 0x8000000000000000);
        CHECK(x == 0x8000000000000000);
    }
#endif
}
