#ifndef BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP

#include <burst/algorithm/sorting/detail/counting_sort.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/variadic.hpp>

#include <algorithm>
#include <climits>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

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

        template <typename Map, typename Radix>
        auto nth_radix (std::size_t radix_number, Map map, Radix radix)
        {
            return [radix_number, map = std::move(map), radix = std::move(radix)] (const auto & value)
            {
                using value_type = std::remove_reference_t<decltype(value)>;
                using traits = radix_sort_traits<value_type, Map, Radix>;

                return radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * radix_number)));
            };
        }

        template <typename ForwardIterator, typename Map, typename Radix, typename Array, std::size_t ... Radices>
        void collect_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix, Array & counters, std::index_sequence<Radices...>)
        {
            std::for_each(first, last,
                [& counters, & map, & radix] (const auto & value)
                {
                    BURST_EXPAND_VARIADIC(++counters[Radices][nth_radix(Radices, map, radix)(value) + 1]);
                });

            BURST_EXPAND_VARIADIC(std::partial_sum(std::begin(counters[Radices]), std::end(counters[Radices]), std::begin(counters[Radices])));
        }

        //!     Собрать счётчики сразу для всех разрядов.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, строго меньших
            этого числа.
         */
        template <typename ForwardIterator, typename Map, typename Radix, typename Array>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Radix radix, Array & counters)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            constexpr auto radix_count = radix_sort_traits<value_type, Map, Radix>::radix_count;
            collect_impl(first, last, map, radix, counters, std::make_index_sequence<radix_count>());
        }

        //!     Специализация для случая, когда в сортируемом числе всего один разряд.
        /*!
                Вызывает сортировку подсчётом из входного диапазона в буфер, а потом переносит
            результат из буфера обратно во входной диапазон.
         */
        template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map, typename Radix>
        typename std::enable_if
        <
            radix_sort_traits
            <
                typename std::iterator_traits<RandomAccessIterator1>::value_type,
                Map,
                Radix
            >
            ::radix_count == 1,
            void
        >
        ::type radix_sort_impl (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer, Map map, Radix radix)
        {
            auto buffer_end = counting_sort_move_impl(first, last, buffer,
                [& map, & radix] (const auto & value)
                {
                    return radix(map(value));
                });

            std::move(buffer, buffer_end, first);
        }

        //!     Специализация для случая, когда количество разрядов сортируемых чисел чётно.
        /*!
                Других случаев не существует по построению. Либо разряд один, либо их количество
            чётно.

                Сортировка происходит между и входным диапазоном и буфером по следующей схеме:
                Перед обработкой каждого чётного разряда (отсчёт от нуля) во входном диапазоне
            лежит неотсортированная по этому разряду последовательность. Она сортируется, и
            результат складывается в буфер.
                Перед обработкой каждого нечётного разряда в буфере лежит неотсортированная
            по этому разряду последовательность. Она сортируется, а результат складывается во
            входной диапазон.
                Таким образом, в итоге во входном диапазоне оказывается отсортированная
            последовательность.
         */
        template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map, typename Radix>
        typename std::enable_if
        <
            radix_sort_traits
            <
                typename std::iterator_traits<RandomAccessIterator1>::value_type,
                Map,
                Radix
            >
            ::radix_count % 2 == 0,
            void
        >
        ::type radix_sort_impl (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer_begin, Map map, Radix radix)
        {
            using value_type = typename std::iterator_traits<RandomAccessIterator1>::value_type;
            using traits = radix_sort_traits<value_type, Map, Radix>;

            using difference_type = typename std::iterator_traits<RandomAccessIterator1>::difference_type;
            difference_type counters[traits::radix_count][traits::radix_value_range + 1] = {{0}};
            collect(first, last, map, radix, counters);

            auto buffer_end = buffer_begin + std::distance(first, last);

            auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
            dispose_move(first, last, buffer_begin, get_low_radix, counters[0]);

            for (std::size_t radix_number = 1; radix_number < traits::radix_count - 1; radix_number += 2)
            {
                dispose_move(buffer_begin, buffer_end, first, nth_radix(radix_number, map, radix), counters[radix_number]);
                dispose_move(first, last, buffer_begin, nth_radix(radix_number + 1, map, radix), counters[radix_number + 1]);
            }

            auto get_high_radix = nth_radix(traits::radix_count - 1, map, radix);
            dispose_move(buffer_begin, buffer_end, first, get_high_radix, counters[traits::radix_count - 1]);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP
