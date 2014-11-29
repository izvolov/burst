#ifndef THRUST_RANGE_MERGE_HPP
#define THRUST_RANGE_MERGE_HPP

#include <boost/range/iterator_range.hpp>

#include <thrust/iterator/merge_iterator.hpp>

namespace thrust
{
    //!     Функция для создания диапазона слияния с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно слить, и операцию, задающую отношение
        строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже должны быть
        упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, состоящий из всех
        элементов входных списков.
     */
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

    //!     Функция для создания диапазона слияний.
    /*!
            Принимает на вход набор диапазонов, которые нужно слить в один.
            Возвращает диапазон, состоящий из всех элементов входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
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
