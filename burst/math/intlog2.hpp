#ifndef BURST_MATH_INTLOG2_HPP
#define BURST_MATH_INTLOG2_HPP

#include <limits>
#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename UnsignedInteger>
        constexpr UnsignedInteger intlog2_impl (UnsignedInteger integer)
        {
            auto degree = std::numeric_limits<UnsignedInteger>::max();

            while (integer > 0)
            {
                integer >>= 1;
                ++degree;
            }

            return degree;
        }
    }

    //!     Целая часть двоичного логарифма.
    template <typename UnsignedInteger>
    constexpr UnsignedInteger intlog2 (UnsignedInteger integer)
    {
        static_assert(std::is_integral<UnsignedInteger>::value, "Число должно быть целым.");
        static_assert(std::is_unsigned<UnsignedInteger>::value, "Число должно быть беззнаковым.");

        return integer > 0
            ? detail::intlog2_impl(integer)
            : throw std::logic_error("Двоичный логарифм не определён на неположительных числах.");
    }
}

#endif // BURST_MATH_INTLOG2_HPP
