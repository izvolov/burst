#ifndef BURST_RANGE_SEMIINTERSECT_HPP
#define BURST_RANGE_SEMIINTERSECT_HPP

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/semiintersect_iterator.hpp>

namespace burst
{
    //!     Функция для создания диапазона полупересечений с предикатом.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение,
        минимальное количество элементов в полупересечениии и операцию, задающую отношение строгого
        порядка на элементах этих диапазонов. При этом сами диапазоны тоже должны быть упорядочены
        относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, каждое значение
        которого соответствует одному элементу, который есть не менее чем в заданном количестве
        входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        semiintersect_iterator<typename ForwardRange::value_type, Compare>
    >
    semiintersect (const ForwardRange & ranges, std::size_t min_items, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_semiintersect_iterator(ranges, min_items, compare),
            make_semiintersect_iterator(ranges, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона полупересечений.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение, и
        минимальное количество элементов в полупересечениии.
            Возвращает диапазон элементов, которые есть не менее чем в заданном количестве входных
        диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    boost::iterator_range
    <
        semiintersect_iterator<typename ForwardRange::value_type>
    >
    semiintersect (const ForwardRange & ranges, std::size_t min_items)
    {
        return boost::make_iterator_range
        (
            make_semiintersect_iterator(ranges, min_items),
            make_semiintersect_iterator(ranges, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона полупересечений из списка инициализации с предикатом.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, для которых нужно найти
        полупересечение, минимальное количество элементов в полупересечениии и отношение строгого
        порядка на элементах этих диапазонов.
            Возвращает упорядоченный относительно заданного отношения порядка диапазон, элементы
        которого есть не менее чем в заданном количестве входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        semiintersect_iterator<ForwardRange, Compare>
    >
    semiintersect (std::initializer_list<ForwardRange> ranges, std::size_t min_items, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_semiintersect_iterator(ranges, min_items, compare),
            make_semiintersect_iterator(ranges, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона полупересечений из списка инициализации.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, для которых нужно найти
        полупересечение, и минимальное количество элементов в полупересечениии.
            Возвращает диапазон элементов, которые есть не менее чем в заданном количестве входных
        диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    boost::iterator_range
    <
        semiintersect_iterator<ForwardRange>
    >
    semiintersect (std::initializer_list<ForwardRange> ranges, std::size_t min_items)
    {
        return boost::make_iterator_range
        (
            make_semiintersect_iterator(ranges, min_items),
            make_semiintersect_iterator(ranges, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_SEMIINTERSECT_HPP
