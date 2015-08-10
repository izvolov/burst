#ifndef BURST_MATH_INTPOW_HPP
#define BURST_MATH_INTPOW_HPP

#include <limits>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        //!     Возведение в степень.
        /*!
                Раскладывает показатель на степени двойки и последовательно перемножает основание в
            тех степенях двойки, которые встретились в этом разложении.
         */
        template <typename Base, typename Exponent>
        constexpr auto intpow_impl (Base x, Exponent n)
        {
            auto result = Base{1};

            while (n > 0)
            {
                if ((n & 0x01) != 0)
                {
                    result *= x;
                }

                x *= x;
                n >>= 1;
            }

            return result;
        }
    } // namespace detail

    //!     Возведение целых чисел в степень.
    /*!
            Так называемый "быстрый алгоритм".
            Время работы: O(logN).
     */
    template <typename Base, typename Exponent>
    constexpr auto intpow (Base x, Exponent n)
    {
        static_assert(std::is_integral<Base>::value, "Основание должно быть целым.");
        static_assert(std::is_integral<Exponent>::value, "Показатель степени должен быть целым.");

        if (n >= 0)
        {
            return detail::intpow_impl(x, n);
        }
        else
        {
            throw std::logic_error("Отрицательная степень целого числа не может быть представлена целым числом.");
        }
    }
} // namespace burst

#endif // BURST_MATH_INTPOW_HPP
