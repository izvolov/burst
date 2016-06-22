#ifndef BURST_RANGE_UNITE_HPP
#define BURST_RANGE_UNITE_HPP

#include <burst/iterator/union_iterator.hpp>

#include <boost/range/iterator_range.hpp>

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
    auto unite (RandomAccessRange && ranges, Compare compare)
    {
        auto begin = make_union_iterator(std::forward<RandomAccessRange>(ranges), compare);
        auto end = make_union_iterator(begin, iterator::end_tag);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }

    //!     Функция для создания диапазона объединений.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить.
            Возвращает диапазон элементов, которые есть хотя бы в одном из входных диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto unite (RandomAccessRange && ranges)
    {
        auto begin = make_union_iterator(std::forward<RandomAccessRange>(ranges));
        auto end = make_union_iterator(begin, iterator::end_tag);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
} // namespace burst

#endif // BURST_RANGE_UNITE_HPP
