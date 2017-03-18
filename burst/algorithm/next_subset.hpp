#ifndef BURST_ALGORITHM_NEXT_SUBSET_HPP
#define BURST_ALGORITHM_NEXT_SUBSET_HPP

#include <algorithm>
#include <functional>
#include <iterator>

namespace burst
{
    namespace detail
    {
        //!     Заполнение подмножества.
        /*!
                Принимает подмножество, заданное полуинтервалом [subset_begin, subset_end),
            последовательность, заданную полуинтервалом [sequence_begin, sequence_end), а также
            отношение порядка на элементах последовательности (сама последовательность должна быть
            упорядочена относительно этой операции).
                Пытается заполнить подмножество итераторами на разные элементы последовательности.
                Возвращает итератор, указывающий за последний заполненный элемент подмножества.
            Если всё подмножество удалось заполнить, то возвращаемый результат равен subset_end.
            Если не всё подмножество удалось заполнить, то результирующий итератор указывает
            куда-то в полуинтервал [subset_begin, subset_end).
         */
        template
        <
            typename ForwardIterator1,
            typename ForwardIterator2,
            typename Compare
        >
        ForwardIterator1
            fill_subset
            (
                ForwardIterator1 subset_begin, ForwardIterator1 subset_end,
                ForwardIterator2 sequence_begin, ForwardIterator2 sequence_end,
                Compare compare
            )
        {
            while (sequence_begin != sequence_end && subset_begin != subset_end)
            {
                *subset_begin++ = sequence_begin;
                sequence_begin = std::upper_bound(sequence_begin, sequence_end, *sequence_begin, compare);
            }

            return subset_begin;
        }

        //!     Переход к следующему подмножеству фиксированного размера.
        /*!
                Принимает подмножество, заданное полуинтервалом [subset_begin, subset_end),
            последовательность, заданную полуинтервалом [sequence_begin, sequence_end), а также
            отношение порядка на элементах последовательности (сама последовательность должна быть
            упорядочена относительно этой операции).
                Пытается найти новое подмножество размера |[subset_begin, subset_end)|.
                Возвращает итератор, указывающий за последний действительный элемент подмножества.
            Если новое подмножество удалось найти, то возвращаемый результат равен subset_end. Если
            не удалось, то результирующий итератор указывает куда-то в полуинтервал
            [subset_begin, subset_end).
         */
        template <typename BidirectionalIterator, typename ForwardIterator, typename Compare>
        BidirectionalIterator
            next_fixed_size_subset
            (
                BidirectionalIterator subset_begin, BidirectionalIterator subset_end,
                ForwardIterator /* sequence_begin */, ForwardIterator sequence_end,
                Compare compare
            )
        {
            auto subset_rbegin = std::make_reverse_iterator(subset_end);
            auto subset_rend = std::make_reverse_iterator(subset_begin);

            auto moving = subset_rbegin;
            while (moving != subset_rend)
            {
                *moving = std::upper_bound(*moving, sequence_end, **moving, compare);
                if (*moving != sequence_end)
                {
                    auto last_filled =
                        fill_subset(std::prev(moving.base()), subset_end, *moving, sequence_end, compare);
                    if (last_filled == subset_end)
                    {
                        return subset_end;
                    }
                }

                ++moving;
            }

            return moving.base();
        }
    } // namespace detail

    //!     Переход к следующему подмножеству.
    /*!
            Принимает последовательность, подмножества которой нужно перебрать, и подмножество
        этой последовательности, которое нужно модифицировать, чтобы получить новое
        подмножество.
            Также получает отношение порядка на элементах последовательности. Сама
        последовательность должна быть упорядочена относительно этой операции.
            Пытается найти новое подмножество и перезаписать им исходное подмножество.
            Если попытка удалась, то в диапазоне подмножества будет лежать новое действительное
        подмножество, и будет возвращён итератор на конец нового подмножества, а если не
        удалась (все подмножества уже перечислены), то возвращается итератор на начало
        подмножества.

            Важно отметить, что в результате работы функции может быть возвращён итератор после
        изначального итератора на конец подмножества. Это значит, что в том буфере, в котором
        лежит само подмножество, должно быть достаточно места для хранения нового подмножества.
     */
    template <typename BidirectionalIterator, typename ForwardIterator, typename Compare>
    BidirectionalIterator
        next_subset
        (
            BidirectionalIterator subset_begin, BidirectionalIterator subset_end,
            ForwardIterator sequence_begin, ForwardIterator sequence_end,
            Compare compare
        )
    {
        if (subset_begin == subset_end
            || detail::next_fixed_size_subset(subset_begin, subset_end, sequence_begin, sequence_end, compare) != subset_end)
        {
            ++subset_end;
            auto last_filled = detail::fill_subset(subset_begin, subset_end, sequence_begin, sequence_end, compare);
            if (last_filled != subset_end)
            {
                subset_end = subset_begin;
            }
        }

        return subset_end;
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
