#ifndef BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP

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
        template <typename Iterator, typename Map>
        struct counting_sort_traits
        {
            using preimage_type = typename std::iterator_traits<Iterator>::value_type;
            using image_type = typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type;
            static_assert
            (
                std::is_integral<image_type>::value,
                "Сортируемые элементы должны быть отображены в целые числа."
            );

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

        //!     Случай, когда выходной итератор — произвольного доступа.
        /*!
                Дополнительный буфер не создаётся, сортированные значения записываются сразу в
            выходной диапазон.
         */
        template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
        void counting_sort_copy_at_a_go (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
        {
            using traits = detail::counting_sort_traits<ForwardIterator, Map>;

            using difference_type = typename std::iterator_traits<RandomAccessIterator>::difference_type;
            // Единица для дополнительного нуля в начале массива.
            difference_type counters[traits::value_range + 1] = {0};

            detail::collect(first, last, map, counters);
            detail::dispose(first, last, result, map, counters);
        }

        //!     Случай, когда выходной итератор не является итератором произвольного доступа.
        /*!
                Создаётся дополнительный буфер, сортированные значения записываются в буфер, а
            затем переносятся в выходной диапазон.
         */
        template <typename ForwardIterator1, typename ForwardIterator2, typename Map>
        void counting_sort_copy_buffered (ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 result, Map map)
        {
            using value_type = typename std::iterator_traits<ForwardIterator1>::value_type;
            std::vector<value_type> buffer(static_cast<std::size_t>(std::distance(first, last)));

            counting_sort_copy_at_a_go(first, last, buffer.begin(), map);
            std::move(buffer.begin(), buffer.end(), result);
        }

        template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
        typename std::enable_if
        <
            std::is_same
            <
                typename std::iterator_traits<RandomAccessIterator>::iterator_category,
                std::random_access_iterator_tag
            >
            ::value,
            void
        >
        ::type counting_sort_copy_impl (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
        {
            counting_sort_copy_at_a_go(first, last, result, map);
        }

        template <typename ForwardIterator1, typename ForwardIterator2, typename Map>
        typename std::enable_if
        <
            not std::is_same
            <
                typename std::iterator_traits<ForwardIterator2>::iterator_category,
                std::random_access_iterator_tag
            >
            ::value,
            void
        >
        ::type counting_sort_copy_impl (ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 result, Map map)
        {
            counting_sort_copy_buffered(first, last, result, map);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_COUNTING_SORT_HPP
