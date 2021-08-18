#ifndef BURST__TUPLE__FORWARD_TUPLE_HPP
#define BURST__TUPLE__FORWARD_TUPLE_HPP

#include <burst/tuple/apply.hpp>
#include <burst/tuple/forward_as_tuple.hpp>

#include <utility>

namespace burst
{
    //!     Пробросить кортеж
    /*!
            Работает аналогично функции `std::forward_as_tuple`, только применяется не к
        произвольным множествам объектов, а к кортежам.

            Принимает произвольный кортеж.
            Возвращает кортеж, состоящий из ссылок на элементы входного кортежа.

            Пусть `Ti` — тип `i`-го элемента входного кортежа. Тогда тип `i`-го элемента исходящего
        кортежа `Ti' = Ti &&`.
     */
    struct forward_tuple_fn
    {
        template <typename T>
        constexpr decltype(auto) operator () (T && t) const
        {
            return burst::apply(forward_as_tuple, std::forward<T>(t));
        }
    };

    constexpr auto forward_tuple = forward_tuple_fn{};
} // namespace burst

#endif // BURST__TUPLE__FORWARD_TUPLE_HPP
