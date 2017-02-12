#ifndef BURST_RANGE_SYMMETRIC_DIFFERENCE_HPP
#define BURST_RANGE_SYMMETRIC_DIFFERENCE_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/symmetric_difference_iterator.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    struct symmetric_difference_t
    {
        //!     Функция для создания диапазона симметрической разности с предикатом
        /*!
                Принимает на вход набор диапазонов, симметрическую разность которых нужно найти, и
            операцию, задающую отношение строгого порядка на элементах этих диапазонов. При этом
            сами диапазоны тоже должны быть упорядочены относительно этой операции.
                Возвращает диапазон, упорядоченный относительно всё той же операции, каждое
            значение которого соответствует одному элементу, который есть в нечётном количестве
            входных диапазонов.
         */
        template <typename RandomAccessRange, typename Compare>
        auto operator () (RandomAccessRange && ranges, Compare compare) const
        {
            auto begin =
                make_symmetric_difference_iterator
                (
                    std::forward<RandomAccessRange>(ranges),
                    compare
                );
            auto end = make_symmetric_difference_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        //!     Функция для создания диапазона симметрической разности
        /*!
                Принимает на вход набор диапазонов, симметрическую разность которых нужно найти.
                Возвращает диапазон элементов, которые есть в нечётном количестве входных
            диапазонов.
                Отношение порядка выбирается по-умолчанию.
         */
        template <typename RandomAccessRange>
        auto operator () (RandomAccessRange && ranges) const
        {
            auto begin = make_symmetric_difference_iterator(std::forward<RandomAccessRange>(ranges));
            auto end = make_symmetric_difference_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto symmetric_difference = symmetric_difference_t{};
}

#endif // BURST_RANGE_SYMMETRIC_DIFFERENCE_HPP
