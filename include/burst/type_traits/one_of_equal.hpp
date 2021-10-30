#ifndef BURST__TYPE_TRAITS__ONE_OF_EQUAL_HPP
#define BURST__TYPE_TRAITS__ONE_OF_EQUAL_HPP

#include <burst/type_traits/are_same.hpp>

#include <type_traits>

namespace burst
{
    /*!
        \brief
            Выбрать один тип из набора одинаковых.

        \returns
            Первый из набора типов, если они одинаковы. В противном случае отваливается по SFINAE.
    */
    template <typename T, typename... Ts>
    struct one_of_equal: std::enable_if<are_same_v<T, Ts...>, T> {};

    template <typename... Ts>
    using one_of_equal_t = typename one_of_equal<Ts...>::type;
} // namespace burst

#endif // BURST__TYPE_TRAITS__ONE_OF_EQUAL_HPP
