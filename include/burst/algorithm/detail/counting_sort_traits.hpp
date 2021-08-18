#ifndef BURST_ALGORITHM_DETAIL_COUNTING_SORT_TRAITS_HPP
#define BURST_ALGORITHM_DETAIL_COUNTING_SORT_TRAITS_HPP

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
        };
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_COUNTING_SORT_TRAITS_HPP
