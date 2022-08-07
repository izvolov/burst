#ifndef BURST__ALGORITHM__SELECT_MIN_HPP
#define BURST__ALGORITHM__SELECT_MIN_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace burst
{
    //!     Поставить минимальные элементы в начало диапазона
    /*!
            По смыслу алгоритм эквивалентен следующей последовательности вызовов:

                auto min = std::min_element(first, last);
                auto count = std::count(first, last, *min);
                std::nth_element(first, first + count - 1, last);

            Но, в отличие от них, работает строго за один проход.

        \tparam ForwardIterator
            Тип итератора принимаемого на вход диапазона. Должен быть хотя бы однонаправленным.
        \tparam BinaryPredicate
            Двухместная операция, задающая отношение строгого порядка на элементах входного
            диапазона. Относительно неё и выбирается наименьший элемент.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<>".
        \return ForwardIterator
            Возвращает итератор на позицию за последним минимальным элементом. Если диапазон пуст,
            то возвращается итератор на его начало.

            Асимптотика.

        Время: O(N), N = |[first, last)| — размер диапазона.
        Память: O(1).
     */
    template <typename ForwardIterator, typename BinaryPredicate>
    auto select_min (ForwardIterator first, ForwardIterator last, BinaryPredicate compare)
    {
        if (first != last)
        {
            auto min = first;

            for (auto current = std::next(first); current != last; ++current)
            {
                if (compare(*current, *min))
                {
                    std::iter_swap(first, current);
                    min = first;
                }
                else if (not compare(*min, *current))
                {
                    ++min;
                    if (min != current)
                    {
                        std::iter_swap(min, current);
                    }
                }
            }

            return ++min;
        }

        return first;
    }

    template <typename ForwardIterator>
    auto select_min (ForwardIterator first, ForwardIterator last)
    {
        return select_min(first, last, std::less<>{});
    }

    template <typename RandomAccessRange, typename BinaryPredicate>
    auto select_min (RandomAccessRange && range, BinaryPredicate compare)
    {
        using std::begin;
        using std::end;
        return
            select_min
            (
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                std::move(compare)
            );
    }

    template <typename RandomAccessRange>
    auto select_min (RandomAccessRange && range)
    {
        return select_min(std::forward<RandomAccessRange>(range), std::less<>{});
    }
} // namespace burst

#endif // BURST__ALGORITHM__SELECT_MIN_HPP
