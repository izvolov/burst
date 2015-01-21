#ifndef BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP
#define BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP

#include <algorithm>
#include <functional>
#include <iterator>

#include <boost/assert.hpp>

namespace burst
{
    template <typename RandomAccessIterator, typename Value, typename Compare>
    RandomAccessIterator galloping_lower_bound (RandomAccessIterator first, RandomAccessIterator last, const Value & value, Compare compare)
    {
        BOOST_ASSERT(std::is_sorted(first, last, compare));

        auto distance = std::distance(first, last);
        using difference_type = typename std::iterator_traits<RandomAccessIterator>::difference_type;
        auto position = static_cast<difference_type>(0);
        auto step = static_cast<difference_type>(1);

        while (position + step < distance)
        {
            auto current = std::next(first, step);
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

    template <typename RandomAccessIterator, typename Value>
    RandomAccessIterator galloping_lower_bound (RandomAccessIterator first, RandomAccessIterator last, const Value & value)
    {
        return galloping_lower_bound(first, last, value, std::less<Value>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP
