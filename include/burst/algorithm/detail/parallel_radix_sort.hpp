#ifndef BURST__ALGORITHM__DETAIL__PARALLEL_RADIX_SORT_HPP
#define BURST__ALGORITHM__DETAIL__PARALLEL_RADIX_SORT_HPP

#include <burst/algorithm/detail/move_assign_please.hpp>
#include <burst/algorithm/detail/nth_radix.hpp>
#include <burst/algorithm/detail/parallel_counting_sort.hpp>
#include <burst/algorithm/detail/radix_sort.hpp>
#include <burst/container/shaped_array_view.hpp>
#include <burst/functional/compose.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/asio/thread_pool.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map, typename Radix>
        typename std::enable_if
        <
            radix_sort_traits
            <
                iterator_value_t<RandomAccessIterator1>,
                Map,
                Radix
            >
            ::radix_count == 1
        >
        ::type radix_sort_impl (boost::asio::thread_pool & pool, const std::array<std::size_t, 2> & shape, RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer, Map map, Radix radix)
        {
            auto buffer_end =
                counting_sort_impl(pool, shape, move_assign_please(first), move_assign_please(last), buffer,
                    [& map, & radix] (const auto & value)
                    {
                        return radix(map(value));
                    });

            std::copy(move_assign_please(buffer), move_assign_please(buffer_end), first);
        }

        template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map, typename Radix>
        typename std::enable_if
        <
            radix_sort_traits
            <
                iterator_value_t<RandomAccessIterator1>,
                Map,
                Radix
            >
            ::radix_count % 2 == 0
        >
        ::type radix_sort_impl (boost::asio::thread_pool & pool, const std::array<std::size_t, 2> & shape, RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer_begin, Map map, Radix radix)
        {
            using value_type = iterator_value_t<RandomAccessIterator1>;
            using traits = radix_sort_traits<value_type, Map, Radix>;
            using difference_type = iterator_difference_t<RandomAccessIterator1>;

            const auto thread_count = shape[0];
            const auto chunk_size = static_cast<difference_type>(shape[1]);

            auto counters = std::make_unique<difference_type[]>(traits::radix_count * thread_count * traits::radix_value_range);
            auto counters_view = make_shaped_array_view(counters.get(), make_shape(traits::radix_count, thread_count, traits::radix_value_range));

            const auto buffer_end = buffer_begin + std::distance(first, last);
            for (auto radix_number = 0ul; radix_number < traits::radix_count; radix_number += 2)
            {
                auto nth = compose(nth_radix(radix_number, radix), map);
                collect(pool, chunk_size, first, last, nth, counters_view[radix_number]);
                dispose_backward(pool, chunk_size, move_assign_please(first), move_assign_please(last), buffer_begin, nth, counters_view[radix_number]);

                auto n1th = compose(nth_radix(radix_number + 1, radix), map);
                collect(pool, chunk_size, buffer_begin, buffer_end, n1th, counters_view[radix_number + 1]);
                dispose_backward(pool, chunk_size, move_assign_please(buffer_begin), move_assign_please(buffer_end), first, n1th, counters_view[radix_number + 1]);
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__PARALLEL_RADIX_SORT_HPP
