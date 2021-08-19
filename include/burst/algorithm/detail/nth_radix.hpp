#ifndef BURST__ALGORITHM__DETAIL__NTH_RADIX_HPP
#define BURST__ALGORITHM__DETAIL__NTH_RADIX_HPP

#include <burst/algorithm/detail/counting_sort_traits.hpp>
#include <burst/integer/right_shift.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace detail
    {
        template <typename Radix>
        auto nth_radix (std::size_t radix_number, Radix radix)
        {
            return
                [radix_number, radix = std::move(radix)] (auto n)
                {
                    using value_type = decltype(n);
                    static_assert(std::is_integral<value_type>::value, "");
                    static_assert(std::is_unsigned<value_type>::value, "");
                    using traits = counting_sort_traits<value_type, Radix>;

                    return radix(right_shift(n, traits::radix_size * radix_number));
                };
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__NTH_RADIX_HPP
