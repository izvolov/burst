#ifndef BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP
#define BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP

#include <burst/container/access/cback.hpp>

#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

namespace burst
{
    namespace detail
    {
        template <typename Value, typename Map>
        struct counting_sort_traits
        {
            using image_type = typename std::decay<typename std::result_of<Map(Value)>::type>::type;
            static_assert
            (
                std::is_integral<image_type>::value && std::is_unsigned<image_type>::value,
                "Сортируемые элементы должны быть отображены в целые числа."
            );

            constexpr static const auto value_range = std::numeric_limits<image_type>::max() + 1;
        };

        //!     Собрать счётчики.
        /*!
                Для каждого сортируемого числа `n_i` подсчитывает количество элементов, которые меньше
            либо равны этому числу, и записывает это число на позицию `counters[n_i]`,
            где `n_i = map(preimage_i)`.
         */
        template <typename ForwardIterator, typename Map, typename RandomAccessIterator>
        void collect (ForwardIterator first, ForwardIterator last, Map map, RandomAccessIterator counters)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using traits = counting_sort_traits<value_type, Map>;

            std::for_each(first, last,
                [& counters, & map] (const auto & preimage)
                {
                    ++counters[map(preimage)];
                });

            const auto counters_end = counters + traits::value_range;
            std::partial_sum(counters, counters_end, counters);
        }

        //!     Расставить по местам.
        /*!
                Расставляет сортируемые элементы в выходной диапазон в соответствии с известным
            массивом счётчиков.
         */
        template <typename ForwardIterator, typename RandomAccessIterator1, typename Map, typename RandomAccessIterator2>
        void dispose (ForwardIterator first, ForwardIterator last, RandomAccessIterator1 result, Map map, RandomAccessIterator2 counters)
        {
            std::for_each(first, last,
                [& result, & counters, & map] (auto && preimage)
                {
                    auto index = counters[map(preimage)]++;
                    result[index] = std::forward<decltype(preimage)>(preimage);
                });
        }

        template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
        RandomAccessIterator counting_sort_impl (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using traits = counting_sort_traits<value_type, Map>;

            using difference_type = typename std::iterator_traits<RandomAccessIterator>::difference_type;
            // Единица для дополнительного нуля в начале массива.
            difference_type counters[traits::value_range + 1] = {0};

            collect(first, last, map, std::next(std::begin(counters)));
            dispose(first, last, result, map, std::begin(counters));

            return result + burst::cback(counters);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_COUNTING_SORT_HPP
