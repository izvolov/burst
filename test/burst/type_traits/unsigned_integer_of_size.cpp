#include <burst/type_traits/unsigned_integer_of_size.hpp>
#include <burst/type_traits/void_t.hpp>

#include <doctest/doctest.h>

#include <boost/cstdint.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace
{
    template <std::size_t Size, typename = void>
    struct is_defined: std::false_type{};

    template <std::size_t Size>
    struct is_defined<Size, burst::void_t<burst::unsigned_integer_of_size_t<Size>>>:
        std::true_type{};
} // namespace

TEST_SUITE("Метафункция unsigned_integer_of_size")
{
    TEST_CASE("Определена для размеров беззнаковых типов")
    {
        CHECK(is_defined<1>::value);
        CHECK(is_defined<2>::value);
        CHECK(is_defined<4>::value);
        CHECK(is_defined<8>::value);
#if defined(BOOST_HAS_INT128)
        CHECK(is_defined<16>::value);
#endif
    }

    TEST_CASE("Не определена для других размеров")
    {
        CHECK(!is_defined<0>::value);
        CHECK(!is_defined<3>::value);
        CHECK(!is_defined<5>::value);
        CHECK(!is_defined<7>::value);
        CHECK(!is_defined<9>::value);
        CHECK(!is_defined<15>::value);
        CHECK(!is_defined<32>::value);
    }

    TEST_CASE("Возвращает стандартные типы, кратные степеням двойки")
    {
        CHECK(std::is_same<burst::unsigned_integer_of_size_t<1>, std::uint8_t>::value);
        CHECK(std::is_same<burst::unsigned_integer_of_size_t<2>, std::uint16_t>::value);
        CHECK(std::is_same<burst::unsigned_integer_of_size_t<4>, std::uint32_t>::value);
        CHECK(std::is_same<burst::unsigned_integer_of_size_t<8>, std::uint64_t>::value);
#if defined(BOOST_HAS_INT128)
        CHECK(std::is_same<burst::unsigned_integer_of_size_t<16>, boost::uint128_type>::value);
#endif
    }
}
