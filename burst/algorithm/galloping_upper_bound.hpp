#ifndef BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP
#define BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP

#include <burst/algorithm/detail/galloping_search.hpp>
#include <burst/functional/non_strict.hpp>

#include <functional>
#include <iterator>
#include <utility>

namespace burst
{
    //!     "Скачущий" поиск верхней грани.
    /*!
            Эффективен для поиска значения в упорядоченном диапазоне, если известно, что искомый
        элемент находится близко к началу диапазона.
            Входной диапазон должен быть либо упорядочен относительно заданного отношения порядка,
        либо, по крайней мере, разделён относительно искомого значения, то есть во входном
        диапазоне все элементы, меньшие искомого относительно заданного отношения порядка, должны
        стоять слева от него, а все, которые не меньше, — справа.

        \tparam RandomAccessIterator
            Тип итератора принимаемого на вход диапазона. Должен быть итератором произвольного
            доступа.
        \tparam Value
            Тип искомого значения. Должен быть сравнимым с элементами диапазона [first, last) в
            качестве левого операнда.
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входного диапазона.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<>".
        \return RandomAccessIterator
            Возвращает итератор на первый элемент в диапазоне, который больше искомого относительно
            заданного порядка, либо, если в диапазоне такого элемента нет, итератор на конец
            диапазона.

            Алгоритм работы.

        1. На первый элемент диапазона устанавливается бегунок.
        2. Устанавливается первоначальный размер скачка (например, 1).
        3. Бегунок совершает скачок.
        4. Элемент, на котором стоит бегунок, сравнивается с искомым.
           а. Если элемент не больше искомого, то размер скачка увеличивается вдвое и бегунок снова
              совершает скачок.
           б. Если бегунок выскочил за пределы диапазона или элемент больше искомого, то искомый
              элемент доискивается двоичным поиском между предыдущим и текущим положением бегунка.

            Асимптотика.

        Время: O(logN), N = |[first, last)| — размер диапазона.
        Память: O(1).
     */
    template <typename RandomAccessIterator, typename Value, typename Compare>
    RandomAccessIterator
        galloping_upper_bound
        (
            RandomAccessIterator first, RandomAccessIterator last,
            const Value & value,
            Compare compare
        )
    {
        return detail::galloping_search(first, last, value, non_strict(compare));
    }

    template <typename RandomAccessIterator, typename Value>
    RandomAccessIterator
        galloping_upper_bound
        (
            RandomAccessIterator first, RandomAccessIterator last,
            const Value & value
        )
    {
        return galloping_upper_bound(first, last, value, std::less<>{});
    }

    template <typename RandomAccessRange, typename Value, typename Compare>
    auto galloping_upper_bound (RandomAccessRange && range, const Value & value, Compare compare)
    {
        return
            galloping_upper_bound
            (
                std::begin(std::forward<RandomAccessRange>(range)),
                std::end(std::forward<RandomAccessRange>(range)),
                value,
                std::move(compare)
            );
    }

    template <typename RandomAccessRange, typename Value>
    auto galloping_upper_bound (RandomAccessRange && range, const Value & value)
    {
        return galloping_upper_bound(std::forward<RandomAccessRange>(range), value, std::less<>{});
    }
} // namespace burst

#endif // BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP
