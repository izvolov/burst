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
        return boost::make_iterator_range
        (
            make_merge_iterator(ranges, compare),
            make_merge_iterator(ranges, compare, iterator::end_tag)
        );
    }

    template <typename RangeRange>
    boost::iterator_range
    <
        merge_iterator<typename RangeRange::value_type>
    >
    merge (const RangeRange & ranges)
    {
        return boost::make_iterator_range
        (
            make_merge_iterator(ranges),
            make_merge_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // THRUST_RANGE_MERGE_HPP
