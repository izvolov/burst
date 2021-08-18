#ifndef BURST__RANGE__ADAPTOR__TO_DEQUE_HPP
#define BURST__RANGE__ADAPTOR__TO_DEQUE_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/container/make_deque.hpp>

namespace burst
{
    struct make_deque_t
    {
        template <typename... Xs>
        constexpr auto operator () (Xs &&... xs) const
        {
            return make_deque(std::forward<Xs>(xs)...);
        }
    };

    /*!
        \brief
            Инструмент для конструирования std::deque через конвейер

        \details
            Вызов

            \code{.cpp}
            range | to_deque
            \endcode

            эквивалентен вызову

            \code{.cpp}
            make_deque(range)
            \endcode

        \see make_deque
     */
    constexpr auto to_deque = make_adaptor_trigger(make_deque_t{});
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__TO_DEQUE_HPP
