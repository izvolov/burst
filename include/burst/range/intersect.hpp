#ifndef BURST_RANGE_INTERSECT_HPP
#define BURST_RANGE_INTERSECT_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/intersect_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct intersect_t
    {
        //!     Функция для создания диапазона пересечений с предикатом.
        /*!
                Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
            отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
            должны быть упорядочены относительно этой операции.
                Возвращает диапазон, упорядоченный относительно всё той же операции, каждое
            значение которого соответствует одному элементу, который есть в каждом из входных
            диапазонов.
         */
        template <typename RandomAccessRange, typename Compare>
        auto operator () (RandomAccessRange && ranges, Compare compare) const
        {
            auto begin = make_intersect_iterator(std::forward<RandomAccessRange>(ranges), compare);
            auto end = make_intersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        //!     Функция для создания диапазона пересечений.
        /*!
                Принимает на вход набор диапазонов, которые нужно пересечь.
                Возвращает диапазон элементов, которые есть в каждом из входных диапазонов.
                Отношение порядка выбирается по-умолчанию.
         */
        template <typename RandomAccessRange>
        auto operator () (RandomAccessRange && ranges) const
        {
            auto begin = make_intersect_iterator(std::forward<RandomAccessRange>(ranges));
            auto end = make_intersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto intersect = intersect_t{};
}

#endif // BURST_RANGE_INTERSECT_HPP
