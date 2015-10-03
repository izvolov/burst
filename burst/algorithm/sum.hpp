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

    template <typename T>
    constexpr
        typename std::enable_if
        <
            std::is_arithmetic<T>::value, T
        >
    ::type sum (std::initializer_list<T> items)
    {
        return detail::sum_impl(items, T{0});
    }
} // namespace burst

#endif // BURST_ALGORITHM_SUM_HPP
