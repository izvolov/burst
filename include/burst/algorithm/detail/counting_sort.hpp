#ifndef BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP
#define BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP

#include <burst/algorithm/detail/cross_partial_sum.hpp>
#include <burst/algorithm/detail/parallel_by_chunks.hpp>
#include <burst/container/access/cback.hpp>
#include <burst/container/shaped_array_view.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/asio/thread_pool.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

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

        template <typename RandomAccessIterator1, typename Map, typename RandomAccessIterator2>
        void
            collect
            (
                boost::asio::thread_pool & pool,
                iterator_difference_t<RandomAccessIterator1> chunk_size,
                RandomAccessIterator1 first,
                RandomAccessIterator1 last,
                Map map,
                RandomAccessIterator2 counters
            )
        {
            using value_type = iterator_value_t<RandomAccessIterator1>;
            constexpr auto radix_value_range = counting_sort_traits<value_type, Map>::value_range;

            const auto chunk_count =
                parallel_by_chunks(pool, chunk_size, first, last,
                    [& counters, & map] (auto thread_index, auto chunk_begin, auto chunk_end)
                    {
                        count(chunk_begin, chunk_end, map, counters[thread_index]);
                    });
            cross_partial_sum(counters, static_cast<std::size_t>(chunk_count), radix_value_range);
        }

        template
        <
            typename RandomAccessIterator1,
            typename RandomAccessIterator2,
            typename Map,
            typename RandomAccessIterator3
        >
        void
            dispose_backward
            (
                boost::asio::thread_pool & pool,
                iterator_difference_t<RandomAccessIterator1> chunk_size,
                RandomAccessIterator1 first,
                RandomAccessIterator1 last,
                RandomAccessIterator2 result,
                Map map,
                RandomAccessIterator3 counters
            )
        {
            parallel_by_chunks(pool, chunk_size, first, last,
                [result, & counters, & map] (auto thread_index, auto chunk_begin, auto chunk_end)
                {
                    dispose_backward(chunk_begin, chunk_end, result, map, counters[thread_index]);
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

        template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
        RandomAccessIterator2
            counting_sort_impl
            (
                boost::asio::thread_pool & pool,
                const std::array<std::size_t, 2> & shape,
                RandomAccessIterator1 first,
                RandomAccessIterator1 last,
                RandomAccessIterator2 result,
                Map map
            )
        {
            using value_type = iterator_value_t<RandomAccessIterator1>;
            using traits = counting_sort_traits<value_type, Map>;
            using difference_type = iterator_difference_t<RandomAccessIterator2>;

            const auto count = shape[0];
            const auto chunk_size =
                static_cast<iterator_difference_t<RandomAccessIterator2>>(shape[1]);

            auto counters = std::make_unique<difference_type[]>(count * traits::value_range);
            auto counters_view =
                make_shaped_array_view(counters.get(), make_shape(count, traits::value_range));

            collect(pool, chunk_size, first, last, map, counters_view);
            dispose_backward(pool, chunk_size, first, last, result, map, counters_view);

            using std::distance;
            return result + distance(first, last);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP
