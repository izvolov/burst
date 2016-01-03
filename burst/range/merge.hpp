#ifndef BURST_RANGE_MERGE_HPP
#define BURST_RANGE_MERGE_HPP

#include <burst/iterator/merge_iterator.hpp>

#include <boost/range/iterator_range.hpp>

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
    template <typename RandomAccessRange, typename Compare>
    boost::iterator_range
    <
        merge_iterator<typename RandomAccessRange::value_type, Compare>
    >
    merge (const RandomAccessRange & ranges, Compare compare)
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
    template <typename RandomAccessRange>
    boost::iterator_range
    <
        merge_iterator<typename RandomAccessRange::value_type>
    >
    merge (const RandomAccessRange & ranges)
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
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        merge_iterator<ForwardRange, Compare>
    >
    merge (std::initializer_list<ForwardRange> ranges, Compare compare)
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
    template <typename ForwardRange>
    boost::iterator_range
    <
        merge_iterator<ForwardRange>
    >
    merge (std::initializer_list<ForwardRange> ranges)
    {
        return boost::make_iterator_range
        (
            make_merge_iterator(ranges),
            make_merge_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_MERGE_HPP
