#ifndef BURST__TYPE_TRAITS__IS_INVOKABLE_HPP
#define BURST__TYPE_TRAITS__IS_INVOKABLE_HPP

#include <burst/type_traits/invoke_result.hpp>
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
            void_t<invoke_result_t<F, As...>>
        >:
        std::true_type {};
}

#endif // BURST__TYPE_TRAITS__IS_INVOKABLE_HPP
