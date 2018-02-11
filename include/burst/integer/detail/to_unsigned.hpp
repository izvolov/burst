#ifndef BURST_INTEGER_DETAIL_TO_UNSIGNED_HPP
#define BURST_INTEGER_DETAIL_TO_UNSIGNED_HPP

#include <limits>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        constexpr auto to_unsigned_impl (bool b)
        {
            return b;
        }

        template <typename I>
        constexpr auto to_unsigned_impl (I n)
        {
            constexpr const auto min_value = std::numeric_limits<I>::min();
            return static_cast<std::make_unsigned_t<I>>(n ^ min_value);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_INTEGER_DETAIL_TO_UNSIGNED_HPP
