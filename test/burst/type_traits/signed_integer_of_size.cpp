#include <burst/type_traits/signed_integer_of_size.hpp>
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
    struct is_defined<Size, burst::void_t<burst::signed_integer_of_size_t<Size>>>:
        std::true_type{};
} // namespace

TEST_SUITE("Метафункция signed_integer_of_size")
{
    TEST_CASE("Определена для размеров знаковых типов")
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
        CHECK(std::is_same<burst::signed_integer_of_size_t<1>, std::int8_t>::value);
        CHECK(std::is_same<burst::signed_integer_of_size_t<2>, std::int16_t>::value);
        CHECK(std::is_same<burst::signed_integer_of_size_t<4>, std::int32_t>::value);
        CHECK(std::is_same<burst::signed_integer_of_size_t<8>, std::int64_t>::value);
#if defined(BOOST_HAS_INT128)
        CHECK(std::is_same<burst::signed_integer_of_size_t<16>, boost::int128_type>::value);
#endif
    }
}
