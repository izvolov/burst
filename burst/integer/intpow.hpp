#ifndef BURST_INTEGER_INTPOW_HPP
#define BURST_INTEGER_INTPOW_HPP

#include <stdexcept>
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
        template <typename Integer1, typename Integer2>
        constexpr auto intpow_impl (Integer1 x, Integer2 exponent)
        {
            auto result = Integer1{1};

            while (exponent > 0)
            {
                if ((exponent & 0x01) != 0)
                {
                    result *= x;
                }

                x *= x;
                exponent >>= 1;
            }

            return result;
        }
    } // namespace detail

    //!     Возведение целых чисел в степень.
    /*!
            Так называемый "быстрый алгоритм".
            Время работы: O(logN).
     */
    template <typename Integer1, typename Integer2>
    constexpr auto intpow (Integer1 base, Integer2 exponent)
    {
        static_assert(std::is_integral<Integer1>::value, "Основание должно быть целым.");
        static_assert(std::is_integral<Integer2>::value, "Показатель степени должен быть целым.");

        return exponent >= 0
            ? detail::intpow_impl(base, exponent)
            : throw std::domain_error("Отрицательная степень целого числа не может быть представлена целым числом");
    }
} // namespace burst

#endif // BURST_INTEGER_INTPOW_HPP
