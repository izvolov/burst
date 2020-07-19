#ifndef BURST_TYPE_TRAITS_ARE_SAME_HPP
#define BURST_TYPE_TRAITS_ARE_SAME_HPP

#include <burst/type_traits/type_list.hpp>

#include <type_traits>

namespace burst
{
    /*!
        \brief
            Проверяет "одинаковость" типов

        \returns
            Истину, когда все переданные типы одинаковы, и ложь в противном случае.
    */
    template <typename T, typename... Ts>
    struct are_same: std::is_same<type_list<T, Ts...>, type_list<Ts..., T>> {};

    template <typename... Ts>
    constexpr const auto are_same_v = are_same<Ts...>::value;
} // namespace burst

#endif // BURST_TYPE_TRAITS_ARE_SAME_HPP
