#ifndef BURST_RANGE_SEMIINTERSECT_HPP
#define BURST_RANGE_SEMIINTERSECT_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/semiintersect_iterator.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    struct semiintersect_t
    {
        //!     Функция для создания диапазона полупересечений с предикатом.
        /*!
                Принимает на вход набор диапазонов, для которых нужно найти полупересечение,
            минимальное количество элементов в полупересечениии и операцию, задающую отношение
            строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже должны быть
            упорядочены относительно этой операции.
                Возвращает диапазон, упорядоченный относительно всё той же операции, каждое
            значение которого соответствует одному элементу, который есть не менее чем в заданном
            количестве входных диапазонов.
         */
        template <typename RandomAccessRange, typename Compare>
        auto operator () (RandomAccessRange && ranges, std::size_t min_items, Compare compare) const
        {
            auto begin =
                make_semiintersect_iterator
                (
                    std::forward<RandomAccessRange>(ranges),
                    min_items,
                    compare
                );
            auto end = make_semiintersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        //!     Функция для создания диапазона полупересечений.
        /*!
                Принимает на вход набор диапазонов, для которых нужно найти полупересечение, и
            минимальное количество элементов в полупересечениии.
                Возвращает диапазон элементов, которые есть не менее чем в заданном количестве
            входных диапазонов.
                Отношение порядка выбирается по-умолчанию.
         */
        template <typename RandomAccessRange>
        auto operator () (RandomAccessRange && ranges, std::size_t min_items) const
        {
            auto begin =
                make_semiintersect_iterator(std::forward<RandomAccessRange>(ranges), min_items);
            auto end = make_semiintersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto semiintersect = semiintersect_t{};
}

#endif // BURST_RANGE_SEMIINTERSECT_HPP
