#ifndef BURST_MATH_INTLOG_HPP
#define BURST_MATH_INTLOG_HPP

#include <limits>
#include <type_traits>

namespace burst
{
    //!     Целая часть логарифма.
    /*!
            Вычисляет целую часть логарифма числа по заданному основанию.
     */
    template <typename UnsignedInteger>
    constexpr UnsignedInteger intlog (UnsignedInteger number, UnsignedInteger base)
    {
        static_assert(std::is_integral<UnsignedInteger>::value, "Число должно быть целым.");
        static_assert(std::is_unsigned<UnsignedInteger>::value, "Число должно быть беззнаковым.");

        UnsignedInteger degree = 0;
        UnsignedInteger exponent = 1;

        while (exponent < number)
        {
            exponent *= base;
            ++degree;
        }

        if (exponent == number)
        {
            return degree;
        }
        else
        {
            return degree - 1;
        }
    }
}

#endif // BURST_MATH_INTLOG_HPP
