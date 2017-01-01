#ifndef BURST_ALGORITHM_DETAIL_GALLOPING_SEARCH_HPP
#define BURST_ALGORITHM_DETAIL_GALLOPING_SEARCH_HPP

#include <algorithm>
#include <iterator>

namespace burst
{
    namespace detail
    {
        template <typename RandomAccessIterator, typename Value, typename Compare>
        RandomAccessIterator
            galloping_search
            (
                RandomAccessIterator first, RandomAccessIterator last,
                const Value & value,
                Compare compare
            )
        {
            const auto distance = std::distance(first, last);

            auto position = static_cast<decltype(distance)>(0);
            auto step = static_cast<decltype(distance)>(1);

            while (position + step < distance)
            {
                const auto current = std::next(first, step);
                if (compare(*current, value))
                {
                    first = std::next(current);
                    position += step + 1;
                    step *= 2;
                }
                else
                {
                    return std::lower_bound(first, current, value, compare);
                }
            }

            return std::lower_bound(first, last, value, compare);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_GALLOPING_SEARCH_HPP
