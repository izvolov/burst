#ifndef BURST__ALGORITHM__DETAIL__COUNTING_SORT_TRAITS_HPP
#define BURST__ALGORITHM__DETAIL__COUNTING_SORT_TRAITS_HPP

#include <burst/integer/intlog2.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Value, typename Map>
        struct counting_sort_traits
        {
            using image_type = std::decay_t<std::result_of_t<Map(Value)>>;
            static_assert
            (
                std::is_integral<image_type>::value && std::is_unsigned<image_type>::value,
                "Сортируемые элементы должны быть отображены в целые числа."
            );

            constexpr static const auto value_range = std::numeric_limits<image_type>::max() + 1;
            constexpr static auto radix_size = intlog2<std::uint64_t>(value_range);
        };
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__COUNTING_SORT_TRAITS_HPP
