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
        typedef intersect_iterator<typename RangeRange::value_type, Compare> iterator;
        return boost::make_iterator_range(iterator(ranges, compare), iterator());
    }

    template <typename RangeRange>
    boost::iterator_range
    <
        intersect_iterator<typename RangeRange::value_type>
    >
    intersect (const RangeRange & ranges)
    {
        typedef intersect_iterator<typename RangeRange::value_type> iterator;
        return boost::make_iterator_range(iterator(ranges), iterator());
    }
}

#endif // THRUST_RANGE_INTERSECT_HPP
