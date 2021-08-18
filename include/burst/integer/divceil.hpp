#ifndef BURST__INTEGER__DIVCEIL_HPP
#define BURST__INTEGER__DIVCEIL_HPP

#include <burst/concept/integer.hpp>
#include <burst/integer/detail/divceil.hpp>

#include <stdexcept>

namespace burst
{
    //!     Целая часть от деления, "округлённая" вверх
    /*!
            Принимает два целых числа: `n` и `divisor`.
            Возвращает наименьшее число `m` такое, что `m * divisor >= n`.
            Тип возвращаемого результата всегда равен типу первого аргумента.
     */
    template <typename I, typename J>
    constexpr auto divceil (Integer<I> n, Integer<J> divisor)
    {
        if (divisor > 0)
        {
            return detail::divceil_impl(n, divisor);
        }
        else
        {
            throw std::domain_error("Целая часть от деления на неположительное число не определена.");
        }
    }
} // namespace burst

#endif // BURST__INTEGER__DIVCEIL_HPP
