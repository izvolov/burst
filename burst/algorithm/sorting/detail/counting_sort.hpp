#ifndef BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP

#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>

#include <burst/algorithm/sorting/detail/radix_sort.hpp>
#include <burst/variadic.hpp>

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

        //!     Собрать счётчики сразу для всех разрядов.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, строго меньших
            этого числа.
         */
        template <typename ForwardIterator, typename Map, typename Radix, typename Array, std::size_t ... Radices>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Radix radix, Array & counters, std::index_sequence<Radices...>)
        {
            using traits = detail::radix_sort_traits<ForwardIterator, Map, Radix>;

            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            std::for_each(first, last,
                [& counters, & map, & radix] (const value_type & value)
                {
                    BURST_VARIADIC(++counters[Radices][radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * Radices))) - traits::min_radix_value + 1]);
                });

            BURST_VARIADIC(std::partial_sum(std::begin(counters[Radices]), std::end(counters[Radices]), std::begin(counters[Radices])));
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
