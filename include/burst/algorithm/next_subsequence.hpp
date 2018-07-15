#ifndef BURST_ALGORITHM_NEXT_SUBSEQUENCE_HPP
#define BURST_ALGORITHM_NEXT_SUBSEQUENCE_HPP

#include <burst/algorithm/next_chain.hpp>

namespace burst
{
    /*!
        \~russian
            \brief
                Поиск следующей подпоследовательности

            \details
                Подпоследовательность представлена в виде полуинтервала
                `[subsequence_begin, subsequence_end)`, элементами которого являются итераторы
                на исходную последовательность, представленную полуинтервалом
                `[sequence_begin, sequence_end)`.

                Функция пытается заполнить подпоследовательность итераторами на элементы
                последовательности.

            \param [subsequence_begin, subsequence_end)
                Полуинтервал, содержащий подпоследовательность.
            \param [sequence_begin, sequence_end)
                Полуинтервал, содержащий исходную последовательность.

            \returns
                Итератор, указывающий за последний заполненный элемент подпоследовательности.
                Если всю подпоследовательность удалось заполнить, то возвращаемый результат
                равен `subsequence_end`.
                Если не всю подпоследовательность удалось заполнить, то результирующий итератор
                указывает куда-то в полуинтервал `[subsequence_begin, subsequence_end)`.
     */
    template <typename BidirectionalIterator, typename ForwardIterator>
    BidirectionalIterator
        next_subsequence
        (
            BidirectionalIterator subsequence_begin,
            BidirectionalIterator subsequence_end,
            ForwardIterator sequence_begin,
            ForwardIterator sequence_end
        )
    {
        return
            next_chain(subsequence_begin, subsequence_end, sequence_begin, sequence_end,
                [] (auto begin, auto &&...)
                {
                    return begin;
                });
    }
} // namespace burst

#endif // BURST_ALGORITHM_NEXT_SUBSEQUENCE_HPP
