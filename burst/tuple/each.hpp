#ifndef BURST_TUPLE_EACH_HPP
#define BURST_TUPLE_EACH_HPP

#include <tuple>
#include <utility>

namespace burst
{
    //!     Применение функции к множеству объектов
    /*!
            Принимает на вход произвольный набор объектов, к каждому из них применяет заданную
        функцию, и результат применений сохраняет в кортеже в соответственном порядке.
     */
    template <typename UnaryFunction>
    struct each_fn
    {
        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) const &
        {
            return std::make_tuple(f(std::forward<Ts>(ts))...);
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) &
        {
            return std::make_tuple(f(std::forward<Ts>(ts))...);
        }

        UnaryFunction f;
    };

    template <typename UnaryFunction>
    constexpr auto each (UnaryFunction && f)
        -> each_fn<std::decay_t<UnaryFunction>>
    {
        return {std::forward<UnaryFunction>(f)};
    }
} // namespace burst

#endif // BURST_TUPLE_EACH_HPP
