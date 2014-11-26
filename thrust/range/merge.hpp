#ifndef THRUST_RANGE_MERGE_HPP
#define THRUST_RANGE_MERGE_HPP

#include <boost/range/iterator_range.hpp>

#include <thrust/iterator/merge_iterator.hpp>

namespace thrust
{
    template <typename RangeRange, typename Compare>
    boost::iterator_range
    <
        merge_iterator<typename RangeRange::value_type, Compare>
    >
    merge (const RangeRange & ranges, Compare compare)
    {
        typedef merge_iterator<typename RangeRange::value_type, Compare> iterator;
        return boost::make_iterator_range(iterator(ranges, compare), iterator());
    }

    template <typename RangeRange>
    boost::iterator_range
    <
        merge_iterator<typename RangeRange::value_type>
    >
    merge (const RangeRange & ranges)
    {
        typedef merge_iterator<typename RangeRange::value_type> iterator;
        return boost::make_iterator_range(iterator(ranges), iterator());
    }
}

#endif // THRUST_RANGE_MERGE_HPP
