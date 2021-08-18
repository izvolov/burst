#ifndef BURST__ALGORITHM__DETAIL__COUNTING_SORT_HPP
#define BURST__ALGORITHM__DETAIL__COUNTING_SORT_HPP

#include <burst/algorithm/detail/counting_sort_traits.hpp>
#include <burst/container/access/cback.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>

namespace burst
{
    namespace detail
    {
        //!     Подсчитать вхождения
        /*!
                Для каждого сортируемого числа `n_i` подсчитывает количество его вхождений в
            диапазон, и записывает это количество на позицию `counters[n_i]`,
            где `n_i = map(first[i])`.
         */
        template <typename ForwardIterator, typename Map, typename RandomAccessIterator>
        void
            count
            (
                ForwardIterator first,
                ForwardIterator last,
                Map map,
                RandomAccessIterator counters
            )
        {
            std::for_each(first, last,
                [& counters, & map] (const auto & preimage)
                {
                    ++counters[map(preimage)];
                });
        }

        //!     Собрать счётчики.
        /*!
                Для каждого сортируемого числа `n_i` подсчитывает количество элементов, которые
            меньше либо равны этому числу, и записывает это число на позицию `counters[n_i]`,
            где `n_i = map(preimage_i)`.
         */
        template <typename ForwardIterator, typename Map, typename RandomAccessIterator>
        void
            collect
            (
                ForwardIterator first,
                ForwardIterator last,
                Map map,
                RandomAccessIterator counters
            )
        {
            using value_type = iterator_value_t<ForwardIterator>;
            using traits = counting_sort_traits<value_type, Map>;

            count(first, last, map, counters);

            const auto counters_end = counters + traits::value_range;
            std::partial_sum(counters, counters_end, counters);
        }

        //!     Расставить по местам.
        /*!
                Расставляет сортируемые элементы в выходной диапазон в соответствии с известным
            массивом счётчиков.
                Важно: в каждый момент времени счётчик `counters[i]` задаёт наименьшую позицию
            элемента `i` в выходном диапазоне.
         */
        template
        <
            typename ForwardIterator,
            typename RandomAccessIterator1,
            typename Map,
            typename RandomAccessIterator2
        >
        void
            dispose
            (
                ForwardIterator first,
                ForwardIterator last,
                RandomAccessIterator1 result,
                Map map,
                RandomAccessIterator2 counters
            )
        {
            std::for_each(first, last,
                [& result, & counters, & map] (auto && preimage)
                {
                    auto index = counters[map(preimage)]++;
                    result[index] = std::forward<decltype(preimage)>(preimage);
                });
        }

        //      Расставить по местам при проходе в обратном порядке
        /*!
                Отличается инвариантом счётчиков: в каждый момент времени счётчик `counters[i]`
            задаёт следующую позицию после наибольшей позиции элемента `i` в выходном диапазоне.
         */
        template
        <
            typename BidirectionalIterator,
            typename RandomAccessIterator1,
            typename Map,
            typename RandomAccessIterator2
        >
        void
            dispose_backward
            (
                BidirectionalIterator first,
                BidirectionalIterator last,
                RandomAccessIterator1 result,
                Map map,
                RandomAccessIterator2 counters
            )
        {
            std::for_each(std::make_reverse_iterator(last), std::make_reverse_iterator(first),
                [& result, & counters, & map] (auto && preimage)
                {
                    auto index = --counters[map(preimage)];
                    result[index] = std::forward<decltype(preimage)>(preimage);
                });
        }

        template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
        RandomAccessIterator
            counting_sort_impl
            (
                ForwardIterator first,
                ForwardIterator last,
                RandomAccessIterator result,
                Map map
            )
        {
            using value_type = iterator_value_t<ForwardIterator>;
            using traits = counting_sort_traits<value_type, Map>;

            using difference_type = iterator_difference_t<RandomAccessIterator>;
            // Единица для дополнительного нуля в начале массива.
            difference_type counters[traits::value_range + 1] = {0};

            collect(first, last, map, std::next(std::begin(counters)));
            dispose(first, last, result, map, std::begin(counters));

            return result + burst::cback(counters);
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__COUNTING_SORT_HPP
