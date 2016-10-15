#ifndef BURST_TUPLE_DETAIL_BY_HPP
#define BURST_TUPLE_DETAIL_BY_HPP

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
                std::make_tuple
                (
                    std::get<BeforeIndex>(std::forward<Tuple>(t))...,
                    std::forward<UnaryFunction>(f)(std::get<Index>(std::forward<Tuple>(t))),
                    std::get<AfterIndex>(std::forward<Tuple>(t))...
                );
        }
    }
} // namespace burst

#endif // BURST_TUPLE_DETAIL_BY_HPP
