#ifndef BURST_RANGE_MERGE_HPP
#define BURST_RANGE_MERGE_HPP

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/merge_iterator.hpp>

namespace burst
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

    //!     Функция для создания диапазона слияний из списка инициализации с предикатом.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно слить, и
        отношение строгого порядка на элементах этих диапазонов.
            Возвращает диапазон, состоящий из всех элементов входных диапазонов, упорядоченный
        относительно заданного отношения порядка.
     */
    template <typename Range, typename Compare>
    boost::iterator_range
    <
        merge_iterator<Range, Compare>
    >
    merge (std::initializer_list<Range> ranges, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_merge_iterator(ranges, compare),
            make_merge_iterator(ranges, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона слияний из списка инициализации.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно слить.
            Возвращает диапазон, состоящий из всех элементов входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename Range>
    boost::iterator_range
    <
        merge_iterator<Range>
    >
    merge (std::initializer_list<Range> ranges)
    {
        return boost::make_iterator_range
        (
            make_merge_iterator(ranges),
            make_merge_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_MERGE_HPP
