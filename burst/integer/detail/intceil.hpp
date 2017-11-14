#ifndef BURST_INTEGER_DETAIL_INTCEIL_HPP
#define BURST_INTEGER_DETAIL_INTCEIL_HPP

#include <burst/integer/detail/divceil.hpp>

namespace burst
{
    namespace detail
    {
        template <typename I, typename J>
        constexpr I intceil_impl (I n, J f)
        {
            return static_cast<I>(detail::divceil_impl(n, f) * static_cast<I>(f));
        }
    }
}

#endif // BURST_INTEGER_DETAIL_INTCEIL_HPP
