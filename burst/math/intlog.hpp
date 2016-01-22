#ifndef BURST_MATH_INTLOG_HPP
#define BURST_MATH_INTLOG_HPP

#include <limits>
#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Integer>
        constexpr Integer intlog_impl (Integer number, Integer base)
        {
            Integer degree = 0;
            Integer exponent = 1;

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
    //!     Целая часть логарифма.
    /*!
            Вычисляет целую часть логарифма числа по заданному основанию.
     */
    template <typename Integer>
    constexpr Integer intlog (Integer number, Integer base)
    {
        static_assert(std::is_integral<Integer>::value, "Число должно быть целым.");

        return
            base > 0
                ? base != 1
                    ? number > 0
                        ? detail::intlog_impl(number, base)
                        : throw std::logic_error("Логарифм не определён на неположительных числах.")
                    : throw std::logic_error("Логарифм по основанию 1 не определён.")
                : throw std::logic_error("Логарифм по неположительному основанию не определён.");
    }
}

#endif // BURST_MATH_INTLOG_HPP
