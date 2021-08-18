#ifndef BURST__TUPLE__GET_HPP
#define BURST__TUPLE__GET_HPP

#include <burst/functional/compose.hpp>

#include <cstddef>
#include <tuple>
#include <utility>

namespace burst
{
    //!     Взятие n-го элемента кортежа
    /*!
            Принимает на вход произвольный кортеж и возвращает его элемент, соответствующий
        заданному индексу `Index`.
     */
    template <std::size_t Index>
    struct get_fn
    {
        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) const
        {
            using std::get;
            return get<Index>(std::forward<Tuple>(t));
        }
    };

    template <std::size_t Index>
    constexpr auto get = get_fn<Index>{};

    template <std::size_t Index, typename R>
    constexpr auto operator | (get_fn<Index> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }

    constexpr auto first = get<0>;
    constexpr auto second = get<1>;
    constexpr auto third = get<2>;
} // namespace burst

#endif // BURST__TUPLE__GET_HPP
