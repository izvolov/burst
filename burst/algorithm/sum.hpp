#ifndef BURST_ALGORITHM_SUM_HPP
#define BURST_ALGORITHM_SUM_HPP

#include <burst/concept/arithmetic.hpp>

#include <initializer_list>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename T>
        constexpr T sum_impl (std::initializer_list<T> items, T value)
        {
            for (const auto & item: items)
            {
                value += item;
            }

            return value;
        }
    }

    struct sum_fn
    {
        template <typename T>
        constexpr decltype(auto) operator () (std::initializer_list<Arithmetic<T>> items) const
        {
            return detail::sum_impl(items, T{0});
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Arithmetic<Ts> ... items) const
        {
            return (*this)({items...});
        }
    };
    constexpr auto sum = sum_fn{};
} // namespace burst

#endif // BURST_ALGORITHM_SUM_HPP
