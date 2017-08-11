#ifndef BURST_INTEGER_INTCEIL_HPP
#define BURST_INTEGER_INTCEIL_HPP

#include <burst/concept/integer.hpp>

#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Integer>
        constexpr Integer intceil_positive (Integer n, Integer f)
        {
            return ((n - Integer{1}) / f) * f + f;
        }

        template <typename Integer>
        constexpr auto intceil_impl (Integer n, Integer f)
            -> std::enable_if_t<std::is_unsigned<Integer>::value, Integer>
        {
            return intceil_positive(n, f);
        }

        template <typename Integer>
        constexpr auto intceil_impl (Integer n, Integer f)
            -> std::enable_if_t<std::is_signed<Integer>::value, Integer>
        {
            if (n > 0)
            {
                return intceil_positive(n, f);
            }
            else
            {
                return (n / f) * f;
            }
        }
    }

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
            return detail::intceil_impl(n, static_cast<I>(factor));
        }
        else
        {
            throw std::domain_error("Ближайшее кратное относительно неположительного делителя не определено.");
        }
    }
}

#endif // BURST_INTEGER_INTCEIL_HPP
