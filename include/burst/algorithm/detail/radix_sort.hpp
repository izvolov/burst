#ifndef BURST__ALGORITHM__DETAIL__RADIX_SORT_HPP
#define BURST__ALGORITHM__DETAIL__RADIX_SORT_HPP

#include <burst/algorithm/detail/counting_sort.hpp>
#include <burst/algorithm/detail/move_assign_please.hpp>
#include <burst/algorithm/detail/nth_radix.hpp>
#include <burst/algorithm/detail/radix_sort_traits.hpp>
#include <burst/functional/compose.hpp>
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
        template <typename InputIterator, typename OutputIterator>
        std::pair<OutputIterator, iterator_value_t<InputIterator>> partial_sum_max (InputIterator first, InputIterator last, OutputIterator result)
        {
            if (first == last) return {result, 0};

            auto max = *first;
            typename std::iterator_traits<InputIterator>::value_type sum = *first;
            *result = sum;

            while (++first != last)
            {
                if (max < *first)
                {
                    max = *first;
                }
                sum = std::move(sum) + *first;
                *++result = sum;
            }
            return {++result, max};
        }

        template <typename ForwardIterator, typename Map, typename Radix, typename RandomAccessIterator1, typename RandomAccessIterator2, std::size_t ... Radices>
        bool collect_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix, RandomAccessIterator1 counters, RandomAccessIterator2 maximums, std::index_sequence<Radices...>)
        {
            using value_type = iterator_value_t<ForwardIterator>;
            constexpr auto radix_value_range = radix_sort_traits<value_type, Map, Radix>::radix_value_range;

            auto previous = std::numeric_limits<std::result_of_t<Map(value_type)>>::min();
            auto is_sorted = true;
            std::for_each(first, last,
                [& counters, & map, & radix, & previous, & is_sorted] (const auto & value)
                {
                    auto current = map(value);
                    is_sorted &= (current >= previous);
                    previous = current;

                    BURST_EXPAND_VARIADIC(++counters[Radices][nth_radix(Radices, radix)(current)]);
                });

            BURST_EXPAND_VARIADIC(maximums[Radices] = partial_sum_max(counters[Radices], counters[Radices] + radix_value_range, counters[Radices]).second);

            return is_sorted;
        }

        //!     Собрать счётчики сразу для всех разрядов.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, которые меньше
            либо равны этому числу.
         */
        template <typename ForwardIterator, typename Map, typename Radix, typename RandomAccessIterator1, typename RandomAccessIterator2>
        bool collect (ForwardIterator first, ForwardIterator last, Map map, Radix radix, RandomAccessIterator1 counters, RandomAccessIterator2 maximums)
        {
            using value_type = iterator_value_t<ForwardIterator>;
            constexpr auto radix_count = radix_sort_traits<value_type, Map, Radix>::radix_count;
            return collect_impl(first, last, map, radix, counters, maximums, std::make_index_sequence<radix_count>());
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
            difference_type maximums[traits::radix_count] = {0};
            const auto is_sorted = collect(first, last, map, radix, counters, maximums);
            if (not is_sorted)
            {
                const auto range_size = std::distance(first, last);
                auto buffer_end = buffer_begin + range_size;
                for (std::size_t radix_number = 0; radix_number < traits::radix_count; radix_number += 2)
                {
                    if (maximums[radix_number] == range_size && maximums[radix_number + 1] == range_size)
                    {
                        continue;
                    }

                    if (maximums[radix_number] == range_size)
                    {
                        std::copy(move_assign_please(first), move_assign_please(last), buffer_begin);
                    }
                    else
                    {
                        dispose_backward(move_assign_please(first), move_assign_please(last), buffer_begin, compose(nth_radix(radix_number, radix), map), std::begin(counters[radix_number]));
                    }

                    if (maximums[radix_number + 1] == range_size)
                    {
                        std::copy(move_assign_please(buffer_begin), move_assign_please(buffer_end), first);
                    }
                    else
                    {
                        dispose_backward(move_assign_please(buffer_begin), move_assign_please(buffer_end), first, compose(nth_radix(radix_number + 1, radix), map), std::begin(counters[radix_number + 1]));
                    }
                }
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__RADIX_SORT_HPP
