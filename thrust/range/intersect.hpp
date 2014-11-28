#ifndef THRUST_RANGE_INTERSECT_HPP
#define THRUST_RANGE_INTERSECT_HPP

#include <boost/range/iterator_range.hpp>

#include <thrust/iterator/intersect_iterator.hpp>

namespace thrust
{
    template <typename RangeRange, typename Compare>
    boost::iterator_range
    <
        intersect_iterator<typename RangeRange::value_type, Compare>
    >
    intersect (const RangeRange & ranges, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_intersect_iterator(ranges, compare),
            make_intersect_iterator(ranges, compare, iterator::end_tag)
        );
    }

    template <typename RangeRange>
    boost::iterator_range
    <
        intersect_iterator<typename RangeRange::value_type>
    >
    intersect (const RangeRange & ranges)
    {
        return boost::make_iterator_range
        (
            make_intersect_iterator(ranges),
            make_intersect_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // THRUST_RANGE_INTERSECT_HPP
