#ifndef BURST_ALGORITHM_DETAIL_RADIX_SORT_HPP
#define BURST_ALGORITHM_DETAIL_RADIX_SORT_HPP

#include <burst/algorithm/detail/counting_sort.hpp>
#include <burst/algorithm/detail/move_assign_please.hpp>
#include <burst/algorithm/detail/nth_radix.hpp>
#include <burst/algorithm/detail/radix_sort_traits.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/variadic.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace detail
    {
        template <typename ForwardIterator, typename Map, typename Radix, typename RandomAccessIterator, std::size_t ... Radices>
        void collect_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix, RandomAccessIterator counters, std::index_sequence<Radices...>)
        {
            using value_type = iterator_value_t<ForwardIterator>;
            constexpr auto radix_value_range = radix_sort_traits<value_type, Map, Radix>::radix_value_range;

            std::for_each(first, last,
                [& counters, & map, & radix] (const auto & value)
                {
                    BURST_EXPAND_VARIADIC(++counters[Radices][nth_radix(Radices, map, radix)(value)]);
                });

            BURST_EXPAND_VARIADIC(std::partial_sum(counters[Radices], counters[Radices] + radix_value_range, counters[Radices]));
        }

        //!     Собрать счётчики сразу для всех разрядов.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, которые меньше
            либо равны этому числу.
         */
        template <typename ForwardIterator, typename Map, typename Radix, typename RandomAccessIterator>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Radix radix, RandomAccessIterator counters)
        {
            using value_type = iterator_value_t<ForwardIterator>;
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
                iterator_value_t<RandomAccessIterator1>,
                Map,
                Radix
            >
            ::radix_count == 1,
            void
        >
        ::type radix_sort_impl (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer, Map map, Radix radix)
        {
            auto buffer_end =
                counting_sort_impl(move_assign_please(first), move_assign_please(last), buffer,
                    [& map, & radix] (const auto & value)
                    {
                        return radix(map(value));
                    });

            std::copy(move_assign_please(buffer), move_assign_please(buffer_end), first);
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
                iterator_value_t<RandomAccessIterator1>,
                Map,
                Radix
            >
            ::radix_count % 2 == 0,
            void
        >
        ::type radix_sort_impl (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer_begin, Map map, Radix radix)
        {
            using value_type = iterator_value_t<RandomAccessIterator1>;
            using traits = radix_sort_traits<value_type, Map, Radix>;

            using difference_type = iterator_difference_t<RandomAccessIterator1>;
            difference_type counters[traits::radix_count][traits::radix_value_range] = {{0}};
            collect(first, last, map, radix, counters);

            auto buffer_end = buffer_begin + std::distance(first, last);
            for (std::size_t radix_number = 0; radix_number < traits::radix_count; radix_number += 2)
            {
                dispose_backward(move_assign_please(first), move_assign_please(last), buffer_begin, nth_radix(radix_number, map, radix), std::begin(counters[radix_number]));
                dispose_backward(move_assign_please(buffer_begin), move_assign_please(buffer_end), first, nth_radix(radix_number + 1, map, radix), std::begin(counters[radix_number + 1]));
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_RADIX_SORT_HPP
