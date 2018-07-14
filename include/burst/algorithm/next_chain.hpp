#ifndef BURST_ALGORITHM_NEXT_CHAIN_HPP
#define BURST_ALGORITHM_NEXT_CHAIN_HPP

#include <iterator>

namespace burst
{
    /*!
        \~russian
            \brief
                Заполнение цепочки

            \details
                Цепочка представлена в виде полуинтервала `[chain_begin, chain_end)`, элементами
                которого являются итераторы на исходную последовательность, представленную
                полуинтервалом `[sequence_begin, sequence_end)`.

                Функция пытается заполнить цепочку итераторами на элементы последовательности,
                найденные последовательными вызовами поисковой функции `find`.

            \param [chain_begin, chain_end)
                Полуинтервал, содержащий цепочку.
            \param [sequence_begin, sequence_end)
                Полуинтервал, содержащий исходную последовательность.
            \param find
                Функция, с помощью которой производится поиск каждого последующего элемента
                цепочки.

            \returns
                Итератор, указывающий за последний заполненный элемент цепочки.
                Если всю цепочку удалось заполнить, то возвращаемый результат равен `chain_end`.
                Если не всю цепочку удалось заполнить, то результирующий итератор указывает
                куда-то в полуинтервал `[chain_begin, chain_end)`.
     */
    template <typename ForwardIterator1, typename ForwardIterator2, typename SearchFunction>
    ForwardIterator1
        fill_chain
        (
            ForwardIterator1 chain_begin,
            ForwardIterator1 chain_end,
            ForwardIterator2 sequence_begin,
            ForwardIterator2 sequence_end,
            SearchFunction find
        )
    {
        while (sequence_begin != sequence_end && chain_begin != chain_end)
        {
            *chain_begin++ = sequence_begin;
            sequence_begin = find(std::next(sequence_begin), sequence_end, *sequence_begin);
        }

        return chain_begin;
    }

    /*!
        \~russian
            \brief
                Переход к следующей цепочке фиксированного размера

            \details
                Цепочка представлена в виде полуинтервала `[chain_begin, chain_end)`, элементами
                которого являются итераторы на исходную последовательность, представленную
                полуинтервалом `[sequence_begin, sequence_end)`.

                Функция пытается найти новую цепочку размера `|[chain_begin, chain_end)|`.

            \param [chain_begin, chain_end)
                Полуинтервал, содержащий цепочку.
            \param [sequence_begin, sequence_end)
                Полуинтервал, содержащий исходную последовательность.
            \param find
                Функция, с помощью которой производится поиск каждого последующего элемента
                цепочки.

            \returns
                Итератор, указывающий за последний действительный элемент цепочки.
                Если новую цепочку удалось найти, то возвращаемый результат равен `chain_end`.
                Если не всю цепочку удалось заполнить, то результирующий итератор указывает
                куда-то в полуинтервал `[chain_begin, chain_end)`.
     */
    template <typename BidirectionalIterator, typename ForwardIterator, typename SearchFunction>
    BidirectionalIterator
        next_fixed_size_chain
        (
            BidirectionalIterator chain_begin,
            BidirectionalIterator chain_end,
            ForwardIterator /* sequence_begin */,
            ForwardIterator sequence_end,
            SearchFunction find
        )
    {
        const auto chain_rbegin = std::make_reverse_iterator(chain_end);
        const auto chain_rend = std::make_reverse_iterator(chain_begin);

        auto moving = chain_rbegin;
        while (moving != chain_rend)
        {
            *moving = find(std::next(*moving), sequence_end, **moving);
            if (*moving != sequence_end)
            {
                const auto last_filled =
                    fill_chain(std::prev(moving.base()), chain_end, *moving, sequence_end, find);
                if (last_filled == chain_end)
                {
                    return chain_end;
                }
            }

            ++moving;
        }

        return moving.base();
    }

    /*!
        \~russian
            \brief
                Переход к следующей цепочке

            \details
                Цепочка представлена в виде полуинтервала `[chain_begin, chain_end)`, элементами
                которого являются итераторы на исходную последовательность, представленную
                полуинтервалом `[sequence_begin, sequence_end)`.

                Функция пытается найти новую цепочку и перезаписать ей исходную цепочку.

            \param [chain_begin, chain_end)
                Полуинтервал, содержащий цепочку.
            \param [sequence_begin, sequence_end)
                Полуинтервал, содержащий исходную последовательность.
            \param find
                Функция, с помощью которой производится поиск каждого последующего элемента
                цепочки.

            \returns
                Итератор на конец новой цепочки, если попытка удалась, и итератор на начало цепочки
                в случае, если попытка оказалось неудачной.
                Также в случае успеха в диапазоне `[chain_begin, chain_end)` будет лежать новая
                цепочка.

                Важно отметить, что в результате работы функции может быть возвращён итератор после
                изначального итератора на конец цепочки. Это значит, что в том буфере, в котором
                лежит сама цепочка, должно быть достаточно места для хранения новой цепочки.
     */
    template <typename BidirectionalIterator, typename ForwardIterator, typename SearchFunction>
    BidirectionalIterator
        next_chain
        (
            BidirectionalIterator chain_begin,
            BidirectionalIterator chain_end,
            ForwardIterator sequence_begin,
            ForwardIterator sequence_end,
            SearchFunction find
        )
    {
        if (chain_begin == chain_end ||
            next_fixed_size_chain
            (
                chain_begin,
                chain_end,
                sequence_begin,
                sequence_end,
                find
            ) != chain_end)
        {
            ++chain_end;
            const auto last_filled =
                fill_chain(chain_begin, chain_end, sequence_begin, sequence_end, find);
            if (last_filled != chain_end)
            {
                chain_end = chain_begin;
            }
        }

        return chain_end;
    }
} // namespace burst

#endif // BURST_ALGORITHM_NEXT_CHAIN_HPP
