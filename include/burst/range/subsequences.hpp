#ifndef BURST__RANGE__SUBSEQUENCES_HPP
#define BURST__RANGE__SUBSEQUENCES_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/subsequence_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    /*!
        \~russian
            \brief
                Функция для создания диапазона подноследовательностей

            \param range
                Диапазон, подноследовательности которого нужно перебрать.
            \param begin
                Диапазон, состоящий из всех подноследовательностей исходного диапазона.

            \returns
                Итератор на пустую подпоследовательность входного диапазона.
     */
    template <typename ForwardRange>
    auto subsequences (ForwardRange && range)
    {
        auto begin = make_subsequence_iterator(std::forward<ForwardRange>(range));
        auto end = make_subsequence_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
}

#endif // BURST__RANGE__SUBSEQUENCES_HPP
