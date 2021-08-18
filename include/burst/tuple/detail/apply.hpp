#ifndef BURST__TUPLE__DETAIL__APPLY_HPP
#define BURST__TUPLE__DETAIL__APPLY_HPP

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

#endif // BURST__TUPLE__DETAIL__APPLY_HPP
