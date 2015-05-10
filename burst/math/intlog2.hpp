#ifndef BURST_MATH_INTLOG2_HPP
#define BURST_MATH_INTLOG2_HPP

#include <limits>
#include <type_traits>

namespace burst
{
    //!     Целая часть двоичного логарифма.
    template <typename UnsignedInteger>
    constexpr UnsignedInteger intlog2 (UnsignedInteger integer)
    {
        static_assert(std::is_integral<UnsignedInteger>::value, "Число должно быть целым.");
        static_assert(std::is_unsigned<UnsignedInteger>::value, "Число должно быть беззнаковым.");

        auto degree = std::numeric_limits<UnsignedInteger>::max();

        while (integer > 0)
        {
            integer >>= 1;
            ++degree;
        }

        return degree;
    }
}

#endif // BURST_MATH_INTLOG2_HPP
