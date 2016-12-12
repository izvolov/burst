#ifndef BURST_ITERATOR_DETAIL_SUBSET_HPP
#define BURST_ITERATOR_DETAIL_SUBSET_HPP

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
        ForwardIterator1 fill_subset
                (
                    ForwardIterator1 subset_begin,
                    ForwardIterator1 subset_end,
                    ForwardIterator2 sequence_begin,
                    ForwardIterator2 sequence_end,
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
        BidirectionalIterator next_fixed_size_subset
                (
                    BidirectionalIterator subset_begin,
                    BidirectionalIterator subset_end,
                    ForwardIterator /* sequence_begin */,
                    ForwardIterator sequence_end,
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
                    auto last_filled = fill_subset(std::prev(moving.base()), subset_end, *moving, sequence_end, compare);
                    if (last_filled == subset_end)
                    {
                        return subset_end;
                    }
                }

                ++moving;
            }

            return moving.base();
        }

        //!     Переход к следующему подмножеству.
        /*!
                Принимает последовательность, подмножества которой нужно перебрать, и подмножество
            этой последовательности, которое нужно модифицировать, чтобы получить новое
            подмножество. Оба заданы контейнерами, причём подмножество — контейнер итераторов на
            исходную последовательность.
                Также получает отношение порядка на элементах последовательности. Сама
            последовательность должна быть упорядочена относительно этой операции.
                Пытается найти новое подмножество и перезаписать им входное подмножество.
                Если попытка удалась, то в аргументе подмножества будет лежать новое действительное
            подмножество, а если не удалась (все подмножества уже перечислены), то подмножество
            станет пустым.
         */
        template <typename ForwardIterator, typename BidirectionalRange, typename Compare>
        void next_subset (ForwardIterator sequence_begin, ForwardIterator sequence_end, BidirectionalRange & subset, Compare compare)
        {
            if (subset.empty() || next_fixed_size_subset(subset.begin(), subset.end(), sequence_begin, sequence_end, compare) != subset.end())
            {
                subset.resize(subset.size() + 1);
                auto last_filled = fill_subset(subset.begin(), subset.end(), sequence_begin, sequence_end, compare);
                if (last_filled != subset.end())
                {
                    subset.clear();
                }
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ITERATOR_DETAIL_SUBSET_HPP
