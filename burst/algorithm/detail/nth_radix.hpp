#ifndef BURST_ALGORITHM_DETAIL_NTH_RADIX_HPP
#define BURST_ALGORITHM_DETAIL_NTH_RADIX_HPP

#include <burst/algorithm/detail/radix_sort_traits.hpp>
#include <burst/integer/right_shift.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace detail
    {
        template <typename Map, typename Radix>
        auto nth_radix (std::size_t radix_number, Map map, Radix radix)
        {
            return
                [radix_number, map = std::move(map), radix = std::move(radix)] (const auto & value)
                {
                    using value_type = std::remove_reference_t<decltype(value)>;
                    using traits = radix_sort_traits<value_type, Map, Radix>;

                    return radix(right_shift(map(value), traits::radix_size * radix_number));
                };
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_NTH_RADIX_HPP
