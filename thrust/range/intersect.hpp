#ifndef THRUST_RANGE_INTERSECT_HPP
#define THRUST_RANGE_INTERSECT_HPP

#include <boost/range/iterator_range.hpp>

#include <thrust/iterator/intersect_iterator.hpp>

namespace thrust
{
    //!     Функция для создания диапазона пересечений с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
        должны быть упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, каждое значение
        которого соответствует одному элементу, который есть в каждом из входных диапазонов.
     */
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

    //!     Функция для создания диапазона пересечений.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь.
            Возвращает диапазон элементов, которые есть в каждом из входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
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
