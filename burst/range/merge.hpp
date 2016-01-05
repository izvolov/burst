#ifndef BURST_RANGE_MERGE_HPP
#define BURST_RANGE_MERGE_HPP

#include <burst/iterator/merge_iterator.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    //!     Функция для создания диапазона слияния с предикатом.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
        должны быть упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, состоящий из всех
        элементов входных списков.
     */
    template <typename RandomAccessRange, typename Compare>
    auto merge (RandomAccessRange ranges, Compare compare)
    {
        return
            boost::make_iterator_range
            (
                make_merge_iterator(std::move(ranges), compare),
                make_merge_iterator(ranges, compare, iterator::end_tag)
            );
    }

    //!     Функция для создания диапазона слияний.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить в один.
            Возвращает диапазон, состоящий из всех элементов входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto merge (RandomAccessRange ranges)
    {
        return
            boost::make_iterator_range
            (
                make_merge_iterator(std::move(ranges)),
                make_merge_iterator(ranges, iterator::end_tag)
            );
    }
}

#endif // BURST_RANGE_MERGE_HPP
