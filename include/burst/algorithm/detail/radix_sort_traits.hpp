#ifndef BURST_ALGORITHM_DETAIL_RADIX_SORT_TRAITS_HPP
#define BURST_ALGORITHM_DETAIL_RADIX_SORT_TRAITS_HPP

#include <burst/integer/intlog2.hpp>

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Value, typename Map, typename Radix>
        struct radix_sort_traits
        {
            using integer_type = std::decay_t<std::result_of_t<Map(Value)>>;
            static_assert
            (
                std::is_integral<integer_type>::value && std::is_unsigned<integer_type>::value,
                "Сортируемые элементы должны быть отображены в целые беззнаковые числа."
            );

            using radix_type = std::decay_t<std::result_of_t<Radix(integer_type)>>;
            static_assert
            (
                std::is_integral<radix_type>::value,
                "Тип разряда, выделяемого из целого числа, тоже должен быть целым."
            );

            constexpr static auto radix_value_range = std::numeric_limits<radix_type>::max() + 1;
            constexpr static auto radix_size = intlog2<std::uint64_t>(radix_value_range);
            constexpr static auto radix_count = sizeof(integer_type) * CHAR_BIT / radix_size;
        };
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_RADIX_SORT_TRAITS_HPP
