#ifndef BURST_ALGORITHM_NEXT_SUBSET_HPP
#define BURST_ALGORITHM_NEXT_SUBSET_HPP

#include <burst/algorithm/next_chain.hpp>

#include <cassert>
#include <algorithm>
#include <functional>

namespace burst
{
    /*!
        \~russian
            \brief
                Поиск следующего подмножества

            \details
                Подмножество представлено в виде полуинтервала `[subset_begin, subset_end)`,
                элементами которого являются итераторы на упорядоченную последовательность,
                представленную полуинтервалом `[sequence_begin, sequence_end)`.

                Функция пытается заполнить подмножество итераторами на элементы
                последовательности.

            \param [subset_begin, subset_end)
                Полуинтервал, содержащий подмножество.
            \param [sequence_begin, sequence_end)
                Полуинтервал, содержащий исходную последовательность.
            \param compare
                Отношение порядка на элементах последовательности. Сама последовательность должна
                быть упорядочена относительно этой операции

            \returns
                Итератор, указывающий за последний заполненный элемент подмножества.
                Если всю подмножество удалось заполнить, то возвращаемый результат
                равен `subset_end`.
                Если не всю подмножество удалось заполнить, то результирующий итератор
                указывает куда-то в полуинтервал `[subset_begin, subset_end)`.
     */
    template <typename BidirectionalIterator, typename ForwardIterator, typename BinaryPredicate>
    BidirectionalIterator
        next_subset
        (
            BidirectionalIterator subset_begin, BidirectionalIterator subset_end,
            ForwardIterator sequence_begin, ForwardIterator sequence_end,
            BinaryPredicate compare
        )
    {
        assert(std::is_sorted(sequence_begin, sequence_end, compare));
        return
            next_chain(subset_begin, subset_end, sequence_begin, sequence_end,
                [& compare] (auto first, auto last, const auto & element)
                {
                    return std::upper_bound(first, last, element, compare);
                });
    }

    template <typename BidirectionalIterator, typename ForwardIterator>
    BidirectionalIterator
        next_subset
        (
            BidirectionalIterator subset_begin, BidirectionalIterator subset_end,
            ForwardIterator sequence_begin, ForwardIterator sequence_end
        )
    {
        return next_subset(subset_begin, subset_end, sequence_begin, sequence_end, std::less<>{});
    }
} // namespace burst

#endif // BURST_ALGORITHM_NEXT_SUBSET_HPP
