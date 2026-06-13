#ifndef BURST__INTEGER__BITWISE_NOT_HPP
#define BURST__INTEGER__BITWISE_NOT_HPP

#include <type_traits>

namespace burst
{
    /*!
        \~english
            \brief
                Bitwise not

            \details
                The only difference with the bitwise negation (`~`) is that the result of the operation has the
                same integral type as the original value.

                Prevents integral promotion.

            \returns
                `static_cast<Integer>(~n)`

        \~russian
            \brief
                Побитовое отрицание

            \details
                Разница с оператором побитового отрицания (`~`) только в том, что результат выражения
                имеет тот же тип, что и исходное число.

                Нужна для избегания неявных преобразований вверх (integral promotion).

            \returns
                `static_cast<Integer>(~n)`
     */
    struct bitwise_not_fn
    {
        template <typename Integer>
        constexpr Integer operator () (Integer n) const
        {
            static_assert
            (
                std::is_integral<Integer>::value,
                "Побитовое отрицание доступно только для целых"
            );
            return static_cast<Integer>(~n);
        }
    };
    constexpr auto bitwise_not = bitwise_not_fn{};
} // namespace burst

#endif // BURST__INTEGER__BITWISE_NOT_HPP
