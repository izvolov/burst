#ifndef BURST__ALGORITHM__DETAIL__PARALLEL_COUNTING_SORT_HPP
#define BURST__ALGORITHM__DETAIL__PARALLEL_COUNTING_SORT_HPP

#include <burst/algorithm/detail/counting_sort.hpp>
#include <burst/algorithm/detail/cross_partial_sum.hpp>
#include <burst/algorithm/detail/parallel_by_chunks.hpp>
#include <burst/container/shaped_array_view.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/asio/thread_pool.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>

namespace burst
{
    namespace detail
    {
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

        template
        <
            typename RandomAccessIterator1,
            typename RandomAccessIterator2,
            typename Map,
            typename RandomAccessIterator3
        >
        RandomAccessIterator2
            counting_sort_impl
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
            collect(pool, chunk_size, first, last, map, counters);
            dispose_backward(pool, chunk_size, first, last, result, map, counters);

            using std::distance;
            using std::next;
            return next(result, distance(first, last));
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

            return counting_sort_impl(pool, chunk_size, first, last, result, map, counters_view);
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__PARALLEL_COUNTING_SORT_HPP
