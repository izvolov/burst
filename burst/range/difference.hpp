#ifndef BURST_RANGE_DIFFERENCE_HPP
#define BURST_RANGE_DIFFERENCE_HPP

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/difference_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

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
    auto difference (const ForwardRange1 & minuend, const ForwardRange2 & subtrahend, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_difference_iterator(minuend, subtrahend, compare),
            make_difference_iterator(minuend, subtrahend, compare, iterator::end_tag)
        );
    }

    //!     Функция для создания разности диапазонов.
    /*!
            Принимает на вход два диапазона, для которых нужно найти разность.
            Возвращает разность диапазонов.
            Отношение порядка выбирается по-умолчанию.
     */
    template <typename ForwardRange1, typename ForwardRange2>
    auto difference (const ForwardRange1 & minuend, const ForwardRange2 & subtrahend)
    {
        return boost::make_iterator_range
        (
            make_difference_iterator(minuend, subtrahend),
            make_difference_iterator(minuend, subtrahend, iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_DIFFERENCE_HPP
