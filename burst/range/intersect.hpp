#ifndef BURST_RANGE_INTERSECT_HPP
#define BURST_RANGE_INTERSECT_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/intersect_iterator.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    //!     Функция для создания диапазона пересечений с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
        должны быть упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, каждое значение
        которого соответствует одному элементу, который есть в каждом из входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        intersect_iterator<typename ForwardRange::value_type, Compare>
    >
    intersect (const ForwardRange & ranges, Compare compare)
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
    template <typename ForwardRange>
    boost::iterator_range
    <
        intersect_iterator<typename ForwardRange::value_type>
    >
    intersect (const ForwardRange & ranges)
    {
        return boost::make_iterator_range
        (
            make_intersect_iterator(ranges),
            make_intersect_iterator(ranges, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона пересечений из списка инициализации с предикатом.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно пересечь,
        и отношение строгого порядка на элементах этих диапазонов.
            Возвращает упорядоченный относительно заданного отношения порядка диапазон, элементы
        которого есть в каждом из входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        intersect_iterator<ForwardRange, Compare>
    >
    intersect (std::initializer_list<ForwardRange> ranges, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_intersect_iterator(ranges, compare),
            make_intersect_iterator(ranges, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона пересечений из списка инициализации.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно пересечь.
            Возвращает диапазон элементов, которые есть в каждом из входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    boost::iterator_range
    <
        intersect_iterator<ForwardRange>
    >
    intersect (std::initializer_list<ForwardRange> ranges)
    {
        return boost::make_iterator_range
        (
            make_intersect_iterator(ranges),
            make_intersect_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_INTERSECT_HPP
