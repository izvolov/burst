#ifndef BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP

#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Iterator, typename Map>
        struct counting_sort_traits
        {
            using preimage_type = typename std::iterator_traits<Iterator>::value_type;
            using image_type = typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type;

            constexpr static const auto min_value = std::numeric_limits<image_type>::min();
            constexpr static const auto max_value = std::numeric_limits<image_type>::max();
            constexpr static const auto value_range = max_value - min_value + 1;
        };

        //!     Собрать счётчики.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, строго меньших
            этого числа.
         */
        template <typename ForwardIterator, typename Map, typename Array>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Array & counters)
        {
            using traits = counting_sort_traits<ForwardIterator, Map>;

            using preimage_type = typename std::iterator_traits<ForwardIterator>::value_type;
            std::for_each(first, last,
                [& counters, & map] (const preimage_type & preimage)
                {
                    ++counters[map(preimage) - traits::min_value + 1];
                });

            std::partial_sum(std::begin(counters), std::end(counters), std::begin(counters));
        }

        //!     Расставить по местам.
        /*!
                Расставляет сортируемые элементы в выходной диапазон в соответствии с известным
            массивом счётчиков.
         */
        template <typename ForwardIterator, typename RandomAccessIterator, typename Map, typename Array>
        void dispose (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map, Array & counters)
        {
            using traits = counting_sort_traits<ForwardIterator, Map>;

            using preimage_type = typename std::iterator_traits<ForwardIterator>::value_type;
            std::for_each(first, last,
                [& result, & counters, & map] (const preimage_type & preimage)
                {
                    auto index = counters[map(preimage) - traits::min_value]++;
                    result[index] = preimage;
                });
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
