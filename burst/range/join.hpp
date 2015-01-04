#ifndef BURST_RANGE_JOIN_HPP
#define BURST_RANGE_JOIN_HPP

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/join_iterator.hpp>

namespace burst
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

    //!     Функция для создания склеенного диапазона из списка инициализации.
    /*!
            Принимает диапазоны для склейки в виде списка инициализации.
            Возвращает склеенный диапазон.
     */
    template <typename Range>
    boost::iterator_range
    <
        join_iterator<Range>
    >
    join (std::initializer_list<Range> ranges)
    {
        return boost::make_iterator_range
        (
            make_join_iterator(ranges),
            make_join_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_JOIN_HPP
