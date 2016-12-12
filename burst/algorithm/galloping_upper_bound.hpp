#ifndef BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP
#define BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP

#include <boost/assert.hpp>

#include <algorithm>
#include <functional>
#include <iterator>

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
    RandomAccessIterator galloping_upper_bound (RandomAccessIterator first, RandomAccessIterator last, const Value & value, Compare compare)
    {
        BOOST_ASSERT(std::is_sorted(first, last, compare));

        const auto distance = std::distance(first, last);

        auto position = static_cast<decltype(distance)>(0);
        auto step = static_cast<decltype(distance)>(1);

        while (position + step < distance)
        {
            const auto current = std::next(first, step);
            if (not compare(value, *current))
            {
                first = std::next(current);
                position += step + 1;
                step *= 2;
            }
            else
            {
                return std::upper_bound(first, current, value, compare);
            }
        }

        return std::upper_bound(first, last, value, compare);
    }

    template <typename RandomAccessIterator, typename Value>
    RandomAccessIterator galloping_upper_bound (RandomAccessIterator first, RandomAccessIterator last, const Value & value)
    {
        return galloping_upper_bound(first, last, value, std::less<>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_GALLOPING_UPPER_BOUND_HPP
