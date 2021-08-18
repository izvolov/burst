#ifndef BURST__FUNCTIONAL__PARENS_HPP
#define BURST__FUNCTIONAL__PARENS_HPP

#include <utility>

namespace burst
{
    //!     Круглоскобочный конструктор
    /*!
            Принимает произвольный набор значений `As...` и инициализирует заданный тип `T` этими
        значениями при помощи круглых скобок: `T(As...)`.
     */
    template <typename T>
    struct parens_fn
    {
        template <typename ... As>
        constexpr auto operator () (As && ... as) const
        {
            return T(std::forward<As>(as)...);
        }
    };

    template <typename T>
    constexpr auto parens = parens_fn<T>{};
}

#endif // BURST__FUNCTIONAL__PARENS_HPP
