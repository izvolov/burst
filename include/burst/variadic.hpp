#ifndef BURST__VARIADIC_HPP
#define BURST__VARIADIC_HPP

#include <initializer_list>

namespace burst
{
    namespace detail
    {
        //!     Функция, которая ничего не делает.
        inline void dummy (std::initializer_list<int>)
        {
        }
    }
}

//!     Выполнение выражения, в котором есть неопределённое количество аргументов.
/*!
        В C++11 и C++14 не компилируется следующий код:

            template <std::size_t I>
            void g (int)
            {
            }

            template <std::size_t ... I>
            void f (std::index_sequence<I...>)
            {
                g<I>(1)...;
            }

        Данный макрос эмулирует подобное поведение:

            template <std::size_t ... I>
            void f (std::index_sequence<I...>)
            {
                BURST_EXPAND_VARIADIC(g<I>(1));
            }

        Вычисление производится слева направо в порядке перечисления аргументов.
 */
#define BURST_EXPAND_VARIADIC(expression) \
    burst::detail::dummy({(expression, 0)...})

#endif // BURST__VARIADIC_HPP
