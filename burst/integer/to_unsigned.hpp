#ifndef BURST_FUNCTIONAL_TO_UNSIGNED_HPP
#define BURST_FUNCTIONAL_TO_UNSIGNED_HPP

#include <burst/concept/integer.hpp>
#include <burst/integer/detail/to_unsigned.hpp>

namespace burst
{
    //!     Преобразование знакового в беззнаковое без потерь
    /*!
            Принимает целое число и преобразует его в беззнаковое целое аналогичного размера.

            Принцип преобразования следующий:
            1. Если полученное число беззнаковое, то с ним ничего не происходит.
            2. Если знаковое, то его диапазон [-x, x) переходит в [0, 2x), а, каждое знаковое `n`
               переходит беззнаковое `n + x`, где x = 2 ^ (sizeof(n) * CHAR_BIT - 1).
     */
    struct to_unsigned_fn
    {
        template <typename I>
        constexpr auto operator () (Integer<I> n) const
        {
            return detail::to_unsigned_impl(n);
        }
    };

    constexpr auto to_unsigned = to_unsigned_fn{};
} // namespace burst

#endif // BURST_FUNCTIONAL_TO_UNSIGNED_HPP
