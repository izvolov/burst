#ifndef BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_RADIX_SORT_HPP

#include <climits>
#include <iterator>
#include <type_traits>

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/low_byte.hpp>
#include <burst/algorithm/sorting/counting_sort.hpp>

namespace burst
{
    template <typename ForwardIterator, typename Map, typename Radix>
    void radix_sort (ForwardIterator first, ForwardIterator last, Map map, Radix radix)
    {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        using integer_type = typename std::decay<typename std::result_of<Map(value_type)>::type>::type;
        static_assert(std::is_integral<integer_type>::value && std::is_unsigned<integer_type>::value,
            "Сортируемые элементы должны быть отображены в целые беззнаковые числа.");

        using radix_type = typename std::decay<typename std::result_of<Radix(integer_type)>::type>::type;
        static_assert(std::is_integral<radix_type>::value,
            "Тип разряда, выделяемого из целого числа, тоже должен быть целым.");

        constexpr const auto min_radix_value = std::numeric_limits<radix_type>::min();
        constexpr const auto max_radix_value = std::numeric_limits<radix_type>::max();
        constexpr const auto radix_value_range = max_radix_value - min_radix_value + 1;
        constexpr const auto radix_size = detail::log2ip(radix_value_range);
        constexpr const auto radix_count = sizeof(integer_type) * CHAR_BIT / radix_size;

        using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
        difference_type counters[radix_count][radix_value_range + 1] = {{0}};
        detail::collect(first, last, map, radix, counters, std::make_index_sequence<radix_count>());

        std::vector<value_type> resulting_buffer(static_cast<std::size_t>(std::distance(first, last)));
        std::vector<value_type> intermediate_buffer(static_cast<std::size_t>(std::distance(first, last)));

        auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
        detail::dispose(first, last, resulting_buffer.begin(), get_low_radix, counters[0]);

        for (std::size_t radix_number = 1; radix_number < radix_count; ++radix_number)
        {
            auto get_radix =
                [& map, & radix, & radix_number] (const value_type & value)
                {
                    return radix(static_cast<integer_type>(map(value) >> (radix_size * radix_number)));
                };
            detail::dispose(resulting_buffer.begin(), resulting_buffer.end(),
                intermediate_buffer.begin(),
                get_radix,
                counters[radix_number]);

            std::swap(resulting_buffer, intermediate_buffer);
        }

        std::move(resulting_buffer.begin(), resulting_buffer.end(), first);
    }

    template <typename ForwardIterator, typename Map>
    void radix_sort (ForwardIterator first, ForwardIterator last, Map map)
    {
        radix_sort(first, last, map, low_byte<>());
    }

    template <typename ForwardIterator>
    void radix_sort (ForwardIterator first, ForwardIterator last)
    {
        radix_sort(first, last, identity<>(), low_byte<>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
