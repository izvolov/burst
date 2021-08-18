#ifndef BURST__FUNCTIONAL__BRACES_HPP
#define BURST__FUNCTIONAL__BRACES_HPP

#include <utility>

namespace burst
{
    //!     Фигурноскобочный конструктор
    /*!
            Принимает произвольный набор значений `As...` и инициализирует заданный тип `T` этими
        значениями при помощи фигурных скобок: `T{As...}`.
     */
    template <typename T>
    struct braces_fn
    {
        template <typename ... As>
        constexpr auto operator () (As && ... as) const
        {
            return T{std::forward<As>(as)...};
        }
    };

    template <typename T>
    constexpr auto braces = braces_fn<T>{};
}

#endif // BURST__FUNCTIONAL__BRACES_HPP
