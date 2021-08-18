#ifndef BURST__TUPLE__DETAIL__BY_HPP
#define BURST__TUPLE__DETAIL__BY_HPP

#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template
        <
            std::size_t Index,
            std::size_t ... BeforeIndex,
            std::size_t ... AfterIndex,
            typename UnaryFunction,
            typename Tuple
        >
        constexpr decltype(auto)
            by_impl
            (
                std::index_sequence<BeforeIndex...>,
                std::index_sequence<AfterIndex...>,
                UnaryFunction && f,
                Tuple && t
            )
        {
            return
                std::tuple
                <
                    std::tuple_element_t<BeforeIndex, std::decay_t<Tuple>>...,
                    std::result_of_t<UnaryFunction(std::tuple_element_t<Index, std::decay_t<Tuple>>)>,
                    std::tuple_element_t<AfterIndex, std::decay_t<Tuple>>...
                >
                (
                    std::get<BeforeIndex>(std::forward<Tuple>(t))...,
                    std::forward<UnaryFunction>(f)(std::get<Index>(std::forward<Tuple>(t))),
                    std::get<AfterIndex>(std::forward<Tuple>(t))...
                );
        }
    }
} // namespace burst

#endif // BURST__TUPLE__DETAIL__BY_HPP
