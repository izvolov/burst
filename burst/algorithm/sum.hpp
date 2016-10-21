#ifndef BURST_ALGORITHM_SUM_HPP
#define BURST_ALGORITHM_SUM_HPP

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
        constexpr
            typename std::enable_if
            <
                std::is_arithmetic<T>::value, T
            >
        ::type operator () (std::initializer_list<T> items) const
        {
            return detail::sum_impl(items, T{0});
        }
    };
    constexpr auto sum = sum_fn{};
} // namespace burst

#endif // BURST_ALGORITHM_SUM_HPP
