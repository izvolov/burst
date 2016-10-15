#ifndef BURST_TUPLE_BY_HPP
#define BURST_TUPLE_BY_HPP

#include <burst/tuple/detail/by.hpp>
#include <burst/type_traits/make_index_range.hpp>

#include <cstddef>
#include <utility>

namespace burst
{
    //!     Преобразовать n-й элемент кортежа
    /*!
            Принимает на вход произвольный кортеж.
            Возвращает новый кортеж такой, что все элементы, кроме заданного индексом `Index`,
        остаются неизменными, а элемент на позиции `Index` является результатом применения функции
        `f` к соответствующему элементу исходного кортежа.
     */
    template <std::size_t Index, typename UnaryFunction, typename Tuple>
    constexpr decltype(auto) by (UnaryFunction && f, Tuple && t)
    {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return
            detail::by_impl<Index>
            (
                make_index_range<0, Index>{},
                make_index_range<Index + 1, size>{},
                std::forward<UnaryFunction>(f),
                std::forward<Tuple>(t)
            );
    }

    template <std::size_t Index, typename UnaryFunction>
    struct by_fn
    {
        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) const &
        {
            return by<Index>(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &
        {
            return by<Index>(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &&
        {
            return by<Index>(std::forward<UnaryFunction>(f), std::forward<Tuple>(t));
        }

        UnaryFunction f;
    };

    template <std::size_t Index, typename UnaryFunction>
    constexpr auto by (UnaryFunction && f)
        -> by_fn<Index, std::decay_t<UnaryFunction>>
    {
        return {std::forward<UnaryFunction>(f)};
    }
} // namespace burst

#endif // BURST_TUPLE_BY_HPP
