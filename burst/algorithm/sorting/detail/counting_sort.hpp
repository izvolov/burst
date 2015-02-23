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
        //!     Собрать счётчики.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, строго меньших
            этого числа.
         */
        template <typename ForwardIterator, typename Map, typename Array>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Array & counters)
        {
            using preimage_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using image_type = typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type;

            constexpr const auto min_value = std::numeric_limits<image_type>::min();

            std::for_each(first, last,
                [& counters, & map] (const preimage_type & preimage)
                {
                    ++counters[map(preimage) - min_value + 1];
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
            using preimage_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using image_type = typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type;

            constexpr const auto min_value = std::numeric_limits<image_type>::min();

            std::for_each(first, last,
                [& result, & counters, & map] (const preimage_type & preimage)
                {
                    auto index = counters[map(preimage) - min_value]++;
                    result[index] = preimage;
                });
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
