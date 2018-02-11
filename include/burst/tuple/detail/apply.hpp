#ifndef BURST_TUPLE_DETAIL_APPLY_HPP
#define BURST_TUPLE_DETAIL_APPLY_HPP

#include <burst/functional/invoke.hpp>

#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename NaryFunction, typename Tuple, std::size_t ... Indices>
        constexpr decltype(auto)
            apply_impl
            (
                NaryFunction && f,
                Tuple && t,
                std::index_sequence<Indices...>
            )
        {
            return invoke(std::forward<NaryFunction>(f), std::get<Indices>(std::forward<Tuple>(t))...);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_TUPLE_DETAIL_APPLY_HPP
