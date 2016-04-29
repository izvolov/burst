#ifndef BURST_CONCEPT_HPP
#define BURST_CONCEPT_HPP

#include <burst/type_traits/void_t.hpp>

#include <type_traits>

namespace burst
{
    template <template <typename ...> class C, typename T, typename = void_t<>>
    struct satisfies: std::false_type{};

    template <template <typename ...> class C, typename T>
    struct satisfies <C, T, void_t<C<T>>>: std::true_type{};

    template <typename Statement>
    using negation = std::integral_constant<bool, !Statement::value>;

    template
    <
        typename T,
        typename =
            std::enable_if_t
            <
                std::is_integral<std::decay_t<T>>::value
            >
    >
    using Integer = T;

    template
    <
        template <typename ...> class C,
        typename T,
        typename =
            std::enable_if_t
            <
                satisfies<C, T>::value
            >
    >
    using Is = T;

    template
    <
        template <typename ...> class C,
        typename T,
        typename =
            std::enable_if_t
            <
                negation<satisfies<C, T>>::value
            >
    >
    using Not = T;
} // namespace burst

#endif // BURST_CONCEPT_HPP
