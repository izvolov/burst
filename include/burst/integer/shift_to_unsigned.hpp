#ifndef BURST__INTEGER__SHIFT_TO_UNSIGNED_HPP
#define BURST__INTEGER__SHIFT_TO_UNSIGNED_HPP

#include <burst/concept/integer.hpp>
#include <burst/integer/detail/shift_to_unsigned.hpp>

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
    struct shift_to_unsigned_fn
    {
        template <typename I>
        constexpr auto operator () (Integer<I> n) const
        {
            return detail::shift_to_unsigned_impl(n);
        }
    };

    constexpr auto shift_to_unsigned = shift_to_unsigned_fn{};
} // namespace burst

#endif // BURST__INTEGER__SHIFT_TO_UNSIGNED_HPP
