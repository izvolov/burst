#ifndef BURST__TYPE_TRAITS__UNSIGNED_INTEGER_OF_SIZE_HPP
#define BURST__TYPE_TRAITS__UNSIGNED_INTEGER_OF_SIZE_HPP

#include <boost/cstdint.hpp>

#include <cstddef>
#include <cstdint>

namespace burst
{
    template <std::size_t Size>
    struct unsigned_integer_of_size
    {
    };

    template <>
    struct unsigned_integer_of_size<1>
    {
        using type = std::uint8_t;
    };

    template <>
    struct unsigned_integer_of_size<2>
    {
        using type = std::uint16_t;
    };

    template <>
    struct unsigned_integer_of_size<4>
    {
        using type = std::uint32_t;
    };

    template <>
    struct unsigned_integer_of_size<8>
    {
        using type = std::uint64_t;
    };

    template <>
    struct unsigned_integer_of_size<16>
    {
#if defined(BOOST_HAS_INT128)
        using type = boost::uint128_type;
#endif
    };

    template <std::size_t Size>
    using unsigned_integer_of_size_t = typename unsigned_integer_of_size<Size>::type;

} // namespace burst

#endif // BURST__TYPE_TRAITS__UNSIGNED_INTEGER_OF_SIZE_HPP
