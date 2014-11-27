#ifndef THRUST_RANGE_SKIP_TO_HPP
#define THRUST_RANGE_SKIP_TO_HPP

#include <boost/range/algorithm/lower_bound.hpp>

namespace thrust
{
    template <typename Range, typename Value, typename Compare>
    void skip_to (Range & range, const Value & goal, Compare compare)
    {
        auto new_begin = boost::lower_bound(range, goal, compare);
        range = boost::make_iterator_range(new_begin, range.end());
    }
}

#endif // THRUST_RANGE_SKIP_TO_HPP
