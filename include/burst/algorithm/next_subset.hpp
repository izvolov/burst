#ifndef BURST_ALGORITHM_NEXT_SUBSET_HPP
#define BURST_ALGORITHM_NEXT_SUBSET_HPP

#include <burst/functional/fn.hpp>

#include <algorithm>
#include <functional>
#include <iterator>

namespace burst
{
    //!     Заполнение цепочки
    /*!
            Принимает цепочку, заданную полуинтервалом [chain_begin, chain_end),
        последовательность, заданную полуинтервалом [sequence_begin, sequence_end), отношение
        порядка на элементах последовательности (сама последовательность должна быть упорядочена
        относительно этой операции), а также функцию, с помощью которой будет производиться поиск
        следующего элемента цепочки.
            Пытается заполнить цепочку итераторами на элементы последовательности, найденные
        последовательными вызовами поисковой функции.
            Возвращает итератор, указывающий за последний заполненный элемент цепочки.
        Если всю цепочку удалось заполнить, то возвращаемый результат равен chain_end.
        Если не всю цепочку удалось заполнить, то результирующий итератор указывает куда-то в
        полуинтервал [chain_begin, chain_end).
     */
    template
    <
        typename ForwardIterator1,
        typename ForwardIterator2,
        typename BinaryPredicate,
        typename SearchFunction
    >
    ForwardIterator1
        fill_chain
        (
            ForwardIterator1 chain_begin, ForwardIterator1 chain_end,
            ForwardIterator2 sequence_begin, ForwardIterator2 sequence_end,
            BinaryPredicate compare,
            SearchFunction find
        )
    {
        while (sequence_begin != sequence_end && chain_begin != chain_end)
        {
            *chain_begin++ = sequence_begin;
            sequence_begin =
                find
                (
                    std::next(sequence_begin),
                    sequence_end,
                    *sequence_begin,
                    compare
                );
        }

        return chain_begin;
    }

    //!     Переход к следующей цепочке фиксированного размера
    /*!
            Принимает цепочку, заданную полуинтервалом [chain_begin, chain_end),
        последовательность, заданную полуинтервалом [sequence_begin, sequence_end), отношение
        порядка на элементах последовательности (сама последовательность должна быть упорядочена
        относительно этой операции), а также функцию, с помощью которой будет производиться поиск
        каждого следующего элемента цепочки.
            Пытается найти новую цепочку размера |[chain_begin, chain_end)|.
            Возвращает итератор, указывающий за последний действительный элемент цепочки.
        Если новую цепочку удалось найти, то возвращаемый результат равен chain_end. Если
        не удалось, то результирующий итератор указывает куда-то в полуинтервал
        [chain_begin, chain_end).
     */
    template
    <
        typename BidirectionalIterator,
        typename ForwardIterator,
        typename BinaryPredicate,
        typename SearchFunction
    >
    BidirectionalIterator
        next_fixed_size_chain
        (
            BidirectionalIterator chain_begin, BidirectionalIterator chain_end,
            ForwardIterator /* sequence_begin */, ForwardIterator sequence_end,
            BinaryPredicate compare,
            SearchFunction find
        )
    {
        const auto chain_rbegin = std::make_reverse_iterator(chain_end);
        const auto chain_rend = std::make_reverse_iterator(chain_begin);

        auto moving = chain_rbegin;
        while (moving != chain_rend)
        {
            *moving = find(std::next(*moving), sequence_end, **moving, compare);
            if (*moving != sequence_end)
            {
                const auto last_filled =
                    fill_chain
                    (
                        std::prev(moving.base()), chain_end,
                        *moving, sequence_end,
                        compare,
                        find
                    );
                if (last_filled == chain_end)
                {
                    return chain_end;
                }
            }

            ++moving;
        }

        return moving.base();
    }

    //!     Переход к следующей цепочке
    /*!
            Принимает последовательность, цепочки в которой нужно перебрать, и диапазон, содержащий
        итераторы некоторой цепочки в этой последовательности, который нужно модифицировать, чтобы
        получить новую цепочку.
            Также получает отношение порядка на элементах последовательности. Сама
        последовательность должна быть упорядочена относительно этой операции.
            Кроме того, получает функцию, с помощью которой будет производиться поиск каждого
        следующего элемента цепочки.
            Пытается найти новую цепочку и перезаписать ей исходную цепочку.
            Если попытка удалась, то в диапазоне цепочки будет лежать новая действительная
        цепочка, и будет возвращён итератор на конец новой цепочки, а если не удалась (все
        цепочки уже перечислены), то возвращается итератор на начало цепочки.

            Важно отметить, что в результате работы функции может быть возвращён итератор после
        изначального итератора на конец цепочки. Это значит, что в том буфере, в котором
        лежит сама цепочка, должно быть достаточно места для хранения новой цепочки.
     */
    template
    <
        typename BidirectionalIterator,
        typename ForwardIterator,
        typename BinaryPredicate,
        typename SearchFunction
    >
    BidirectionalIterator
        next_chain
        (
            BidirectionalIterator chain_begin, BidirectionalIterator chain_end,
            ForwardIterator sequence_begin, ForwardIterator sequence_end,
            BinaryPredicate compare,
            SearchFunction find
        )
    {
        if (chain_begin == chain_end ||
            next_fixed_size_chain
            (
                chain_begin, chain_end,
                sequence_begin, sequence_end,
                compare,
                find
            ) != chain_end)
        {
            ++chain_end;
            const auto last_filled =
                fill_chain(chain_begin, chain_end, sequence_begin, sequence_end, compare, find);
            if (last_filled != chain_end)
            {
                chain_end = chain_begin;
            }
        }

        return chain_end;
    }

    template <typename BidirectionalIterator, typename ForwardIterator, typename BinaryPredicate>
    BidirectionalIterator
        next_subset
        (
            BidirectionalIterator subset_begin, BidirectionalIterator subset_end,
            ForwardIterator sequence_begin, ForwardIterator sequence_end,
            BinaryPredicate compare
        )
    {
        return
            next_chain
            (
                subset_begin, subset_end,
                sequence_begin, sequence_end,
                compare,
                BURST_FN(std::upper_bound)
            );
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
