#ifndef BURST_RANGE_UNITE_HPP
#define BURST_RANGE_UNITE_HPP

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/union_iterator.hpp>

namespace burst
{
    //!     Функция для создания диапазона объединений с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
        должны быть упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, каждое значение
        которого соответствует одному элементу, который есть хотя бы в одном из входных диапазонов.
     */
    template <typename RandomAccessRange, typename Compare>
    boost::iterator_range
    <
        union_iterator<typename RandomAccessRange::value_type, Compare>
    >
    unite (RandomAccessRange && ranges, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_union_iterator(std::forward<RandomAccessRange>(ranges), compare),
            make_union_iterator(std::forward<RandomAccessRange>(ranges), compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона объединений.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить.
            Возвращает диапазон элементов, которые есть хотя бы в одном из входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    boost::iterator_range
    <
        union_iterator<typename RandomAccessRange::value_type>
    >
    unite (RandomAccessRange && ranges)
    {
        return boost::make_iterator_range
        (
            make_union_iterator(std::forward<RandomAccessRange>(ranges)),
            make_union_iterator(std::forward<RandomAccessRange>(ranges), iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона объединений из списка инициализации с предикатом.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно
        объединить, и отношение строгого порядка на элементах этих диапазонов.
            Возвращает упорядоченный относительно заданного отношения порядка диапазон, элементы
        которого есть хотя бы в одном из входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    boost::iterator_range
    <
        union_iterator<ForwardRange, Compare>
    >
    unite (std::initializer_list<ForwardRange> ranges, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_union_iterator(ranges, compare),
            make_union_iterator(ranges, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания диапазона объединений из списка инициализации.
    /*!
            Принимает на вход в виде списка инициализации набор диапазонов, которые нужно
        объединить.
            Возвращает диапазон элементов, которые есть хотя бы в одном из входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    boost::iterator_range
    <
        union_iterator<ForwardRange>
    >
    unite (std::initializer_list<ForwardRange> ranges)
    {
        return boost::make_iterator_range
        (
            make_union_iterator(ranges),
            make_union_iterator(ranges, iterator::end_tag)
        );
    }
} // namespace burst

#endif // BURST_RANGE_UNITE_HPP
