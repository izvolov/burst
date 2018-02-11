#ifndef BURST_RANGE_MERGE_HPP
#define BURST_RANGE_MERGE_HPP

#include <burst/iterator/merge_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct merge_t
    {
        //!     Функция для создания диапазона слияния с предикатом.
        /*!
                Принимает на вход диапазон диапазонов, которые нужно слить, и операцию, задающую
            отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
            должны быть упорядочены относительно этой операции.
                Возвращает диапазон, упорядоченный относительно всё той же операции, состоящий из
            всех элементов входных списков.
         */
        template <typename RandomAccessRange, typename Compare>
        auto operator () (RandomAccessRange && ranges, Compare compare) const
        {
            auto begin = make_merge_iterator(std::forward<RandomAccessRange>(ranges), compare);
            auto end = make_merge_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        //!     Функция для создания диапазона слияний.
        /*!
                Принимает на вход диапазон диапазонов, которые нужно слить в один.
                Возвращает диапазон, состоящий из всех элементов входных диапазонов.
                Отношение порядка выбирается по-умолчанию.
         */
        template <typename RandomAccessRange>
        auto operator () (RandomAccessRange && ranges) const
        {
            auto begin = make_merge_iterator(std::forward<RandomAccessRange>(ranges));
            auto end = make_merge_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto merge = merge_t{};
}

#endif // BURST_RANGE_MERGE_HPP
