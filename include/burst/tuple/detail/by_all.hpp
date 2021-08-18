#ifndef BURST__TUPLE__DETAIL__BY_ALL_HPP
#define BURST__TUPLE__DETAIL__BY_ALL_HPP

#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <std::size_t... Indices, typename UnaryFunction, typename Tuple>
        constexpr decltype(auto)
            by_all_impl (std::index_sequence<Indices...>, UnaryFunction & f, Tuple && t)
        {
            return
                std::tuple
                <
                    std::result_of_t
                    <
                        UnaryFunction &(std::tuple_element_t<Indices, std::decay_t<Tuple>>)
                    >...
                >
                (
                    f(std::get<Indices>(std::forward<Tuple>(t)))...
                );
        }

        template <typename UnaryFunction, typename Tuple>
        constexpr decltype(auto) by_all_impl (const UnaryFunction & f, Tuple && t)
        {
            constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
            return by_all_impl(std::make_index_sequence<size>{}, f, std::forward<Tuple>(t));
        }

        template <typename UnaryFunction, typename Tuple>
        constexpr decltype(auto) by_all_impl (UnaryFunction & f, Tuple && t)
        {
            constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
            return by_all_impl(std::make_index_sequence<size>{}, f, std::forward<Tuple>(t));
        }
    }
} // namespace burst

#endif // BURST__TUPLE__DETAIL__BY_ALL_HPP
