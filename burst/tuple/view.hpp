#ifndef BURST_TUPLE_VIEW_HPP
#define BURST_TUPLE_VIEW_HPP

#include <burst/functional/compose.hpp>
#include <burst/tuple/get.hpp>

#include <cstddef>
#include <tuple>
#include <utility>

namespace burst
{
    //!     Просмотр подмножества кортежа
    /*!
            Принимает на вход произвольный кортеж и возвращает кортеж, состоящий из ссылок на
        элементы исходного кортежа, соответствующие заданному набору индексов `Indices...`.
     */
    template <std::size_t ... Indices>
    struct view_fn
    {
        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) const
        {
            return std::forward_as_tuple(get<Indices>(std::forward<Tuple>(t))...);
        }
    };

    template <std::size_t ... Indices>
    constexpr auto view = view_fn<Indices...>{};

    template <std::size_t ... Indices, typename R>
    constexpr auto operator | (view_fn<Indices...> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST_TUPLE_VIEW_HPP
