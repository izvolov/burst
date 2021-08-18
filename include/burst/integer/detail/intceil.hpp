#ifndef BURST__INTEGER__DETAIL__INTCEIL_HPP
#define BURST__INTEGER__DETAIL__INTCEIL_HPP

#include <burst/integer/detail/divceil.hpp>

namespace burst
{
    namespace detail
    {
        template <typename I, typename J>
        constexpr I intceil_impl (I n, J f)
        {
            // Выражение вынесено в отдельную переменную исключительно ради ГЦЦ, который
            // почему-то никак иначе не может. Вернуть обратно, когда сможет.
            const auto divceil = detail::divceil_impl(n, f);
            return static_cast<I>(divceil * static_cast<I>(f));
        }
    }
}

#endif // BURST__INTEGER__DETAIL__INTCEIL_HPP
