#ifndef BURST_TUPLE_GET_HPP
#define BURST_TUPLE_GET_HPP

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

    constexpr auto first = get<0>;
    constexpr auto second = get<1>;
    constexpr auto third = get<2>;
} // namespace burst

#endif // BURST_TUPLE_GET_HPP
