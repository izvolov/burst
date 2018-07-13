#ifndef BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP
#define BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP

#include <burst/algorithm/detail/galloping_search.hpp>

#include <functional>
#include <iterator>
#include <utility>

namespace burst
{
    //!     "Скачущий" поиск нижней грани.
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
            качестве правого операнда.
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входного диапазона.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<>".
        \return RandomAccessIterator
            Возвращает итератор на первый элемент в диапазоне, который не меньше искомого
            относительно заданного порядка, либо, если если в диапазоне такого элемента нет,
            итератор на конец диапазона.

            Алгоритм работы.

        1. На первый элемент диапазона устанавливается бегунок.
        2. Устанавливается первоначальный размер скачка (например, 1).
        3. Бегунок совершает скачок.
        4. Элемент, на котором стоит бегунок, сравнивается с искомым.
           а. Если элемент меньше искомого, то размер скачка увеличивается вдвое и бегунок снова
              совершает скачок.
           б. Если бегунок выскочил за пределы диапазона или элемент не меньше искомого, то искомый
              элемент доискивается двоичным поиском между предыдущим и текущим положением бегунка.

            Асимптотика.

        Время: O(logN), N = |[first, last)| — размер диапазона.
        Память: O(1).
     */
    template <typename RandomAccessIterator, typename Value, typename Compare>
    RandomAccessIterator
        galloping_lower_bound
        (
            RandomAccessIterator first, RandomAccessIterator last,
            const Value & value,
            Compare compare
        )
    {
        return detail::galloping_search(first, last, value, compare);
    }

    template <typename RandomAccessIterator, typename Value>
    RandomAccessIterator
        galloping_lower_bound
        (
            RandomAccessIterator first, RandomAccessIterator last,
            const Value & value
        )
    {
        return galloping_lower_bound(first, last, value, std::less<>{});
    }

    template <typename RandomAccessRange, typename Value, typename Compare>
    auto galloping_lower_bound (RandomAccessRange && range, const Value & value, Compare compare)
    {
        using std::begin;
        using std::end;
        return
            galloping_lower_bound
            (
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                value,
                std::move(compare)
            );
    }

    template <typename RandomAccessRange, typename Value>
    auto galloping_lower_bound (RandomAccessRange && range, const Value & value)
    {
        return galloping_lower_bound(std::forward<RandomAccessRange>(range), value, std::less<>{});
    }
} // namespace burst

#endif // BURST_ALGORITHM_GALLOPING_LOWER_BOUND_HPP
