#ifndef BURST_TUPLE_BY_ALL_HPP
#define BURST_TUPLE_BY_ALL_HPP

#include <burst/functional/compose.hpp>
#include <burst/tuple/detail/by_all.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace burst
{
    //!     Преобразовать все элементы кортежа
    /*!
            Принимает на вход произвольный кортеж.
            Возвращает новый кортеж такой, что каждый его элемент является результатом применения
        функции `f` к соответствующему элементу исходного кортежа.
     */
    template <typename UnaryFunction, typename Tuple>
    constexpr decltype(auto) by_all (UnaryFunction && f, Tuple && t)
    {
        return detail::by_all_impl(std::forward<UnaryFunction>(f), std::forward<Tuple>(t));
    }

    template <typename UnaryFunction>
    struct by_all_fn
    {
        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) const &
        {
            return by_all(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &
        {
            return by_all(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &&
        {
            return by_all(f, std::forward<Tuple>(t));
        }

        UnaryFunction f;
    };

    template <typename UnaryFunction>
    constexpr auto by_all (UnaryFunction && f)
        -> by_all_fn<std::decay_t<UnaryFunction>>
    {
        return {std::forward<UnaryFunction>(f)};
    }

    template <typename UnaryFunction, typename R>
    constexpr auto operator | (by_all_fn<UnaryFunction> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST_TUPLE_BY_ALL_HPP
