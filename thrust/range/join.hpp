#ifndef THRUST_RANGE_JOIN_HPP
#define THRUST_RANGE_JOIN_HPP

#include <boost/range/iterator_range.hpp>

#include <thrust/iterator/end_tag.hpp>
#include <thrust/iterator/join_iterator.hpp>

namespace thrust
{
    //!     Функция для создания склеенного диапазона.
    /*!
            Принимает на вход набор диапазонов, которые нужно склеить.
            Возвращает диапазон, представляющий из себя один большой диапазон, в котором входные
        диапазоны поставлены друг за другом таким образом, что после последнего элемента i-го
        диапазона сразу следует первый элемент (i + 1)-го.
     */
    template <typename RangeRange>
    boost::iterator_range
    <
        join_iterator<typename RangeRange::value_type>
    >
    join (const RangeRange & ranges)
    {
        return boost::make_iterator_range
        (
            make_join_iterator(ranges),
            make_join_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // THRUST_RANGE_JOIN_HPP
