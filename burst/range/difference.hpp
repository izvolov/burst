#ifndef BURST_RANGE_DIFFERENCE_HPP
#define BURST_RANGE_DIFFERENCE_HPP

#include <burst/iterator/difference_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    //!     Функция для создания разности диапазонов с предикатом.
    /*!
            Принимает на вход два диапазона — уменьшаемое и вычитаемое в соответствующем порядке –
        и отношение строгого порядка на элементах этих диапазонов. При этом сами диапазоны тоже
        должны быть упорядочены относительно этой операции.
            Возвращает диапазон, упорядоченный относительно всё той же операции, каждый элемент
        которого одновременно есть в уменьшаемом и отсутствует в вычитаемом.
     */
    template <typename ForwardRange1, typename ForwardRange2, typename Compare>
    auto difference (ForwardRange1 minuend, ForwardRange2 subtrahend, Compare compare)
    {
        auto begin = make_difference_iterator(std::move(minuend), std::move(subtrahend), compare);
        auto end = make_difference_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }

    //!     Функция для создания разности диапазонов.
    /*!
            Принимает на вход два диапазона, для которых нужно найти разность.
            Возвращает разность диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange1, typename ForwardRange2>
    auto difference (ForwardRange1 minuend, ForwardRange2 subtrahend)
    {
        auto begin = make_difference_iterator(std::move(minuend), std::move(subtrahend));
        auto end = make_difference_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
}

#endif // BURST_RANGE_DIFFERENCE_HPP
