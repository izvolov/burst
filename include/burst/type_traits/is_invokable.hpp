#ifndef BURST_TYPE_TRAITS_IS_INVOKABLE_HPP
#define BURST_TYPE_TRAITS_IS_INVOKABLE_HPP

#include <burst/type_traits/void_t.hpp>

#include <type_traits>

namespace burst
{
    template <typename, typename = void_t<>>
    struct is_invokable: std::false_type {};

    template <typename F, typename ... As>
    struct is_invokable
        <
            F(As...),
            void_t<std::result_of_t<F(As...)>>
        >:
        std::true_type {};
}

#endif // BURST_TYPE_TRAITS_IS_INVOKABLE_HPP
