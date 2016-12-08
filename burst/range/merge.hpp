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
    auto merge (RandomAccessRange && ranges, Compare compare)
    {
        auto begin = make_merge_iterator(std::forward<RandomAccessRange>(ranges), compare);
        auto end = make_merge_iterator(begin, iterator::end_tag);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }

    //!     Функция для создания диапазона слияний.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить в один.
            Возвращает диапазон, состоящий из всех элементов входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto merge (RandomAccessRange && ranges)
    {
        auto begin = make_merge_iterator(std::forward<RandomAccessRange>(ranges));
        auto end = make_merge_iterator(begin, iterator::end_tag);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
}

#endif // BURST_RANGE_MERGE_HPP
