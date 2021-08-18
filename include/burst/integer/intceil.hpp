#ifndef BURST__INTEGER__INTCEIL_HPP
#define BURST__INTEGER__INTCEIL_HPP

#include <burst/concept/integer.hpp>
#include <burst/integer/detail/intceil.hpp>

#include <stdexcept>

namespace burst
{
    //!     Ближайшее кратное
    /*!
            Принимает два целых числа: `n` и `factor`.
            Возвращает наименьшее число `m` такое, что `m >= n` и `m` делится на `factor`.

            Если ближайшее кратное непредставимо типом `I`, то поведение неопределено.
     */
    template <typename I, typename J>
    constexpr I intceil (Integer<I> n, Integer<J> factor)
    {
        if (factor > 0)
        {
            return detail::intceil_impl(n, factor);
        }
        else
        {
            throw std::domain_error("Ближайшее кратное относительно неположительного делителя не определено.");
        }
    }
}

#endif // BURST__INTEGER__INTCEIL_HPP
