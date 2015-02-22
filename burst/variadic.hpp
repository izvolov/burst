#ifndef BURST_VARIADIC_HPP
#define BURST_VARIADIC_HPP

namespace burst
{
    namespace detail
    {
        //!     Функция, которая ничего не делает.
        template <typename ... Arguments>
        void dummy (Arguments ...)
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
                BURST_VARIADIC(g<I>(1));
            }
 */
#define BURST_VARIADIC(expression) \
    burst::detail::dummy((expression, 0)...)

#endif // BURST_VARIADIC_HPP
