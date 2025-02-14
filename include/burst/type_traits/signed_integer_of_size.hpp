#ifndef BURST__TYPE_TRAITS__SIGNED_INTEGER_OF_SIZE_HPP
#define BURST__TYPE_TRAITS__SIGNED_INTEGER_OF_SIZE_HPP

#include <boost/cstdint.hpp>

#include <cstddef>
#include <cstdint>

namespace burst
{
    template <std::size_t Size>
    struct signed_integer_of_size
    {
    };

    template <>
    struct signed_integer_of_size<1>
    {
        using type = std::int8_t;
    };

    template <>
    struct signed_integer_of_size<2>
    {
        using type = std::int16_t;
    };

    template <>
    struct signed_integer_of_size<4>
    {
        using type = std::int32_t;
    };

    template <>
    struct signed_integer_of_size<8>
    {
        using type = std::int64_t;
    };

    template <>
    struct signed_integer_of_size<16>
    {
#if defined(BOOST_HAS_INT128)
        using type = boost::int128_type;
#endif
    };

    template <std::size_t Size>
    using signed_integer_of_size_t = typename signed_integer_of_size<Size>::type;

} // namespace burst

#endif // BURST__TYPE_TRAITS__SIGNED_INTEGER_OF_SIZE_HPP
