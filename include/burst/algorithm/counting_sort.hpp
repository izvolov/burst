#ifndef BURST_ALGORITHM_COUNTING_SORT_HPP
#define BURST_ALGORITHM_COUNTING_SORT_HPP

#include <burst/algorithm/detail/counting_sort.hpp>
#include <burst/functional/compose.hpp>
#include <burst/functional/identity.hpp>
#include <burst/integer/to_unsigned.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    //!     Сортировка подсчётом.
    /*!
            Алгоритм целочисленной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение сортируемых целых чисел. Максимальное значение вычисляется на этапе компиляции,
        исходя из типа, возвращаемого отображением "Map".
            В версии "_copy" результат сортировки записывается в выходной диапазон, а входной
        диапазон остаётся без изменений.
            В версии "_move" элементы входного диапазона в результате сортировки перемещаются в
        выходной.
            Выходной диапазон должен быть не меньше входного, чтобы в него влезли
        отсортированные данные.

        \tparam ForwardIterator
            Тип принимаемого на вход диапазона, который нужно отсортировать. Для него достаточно
            быть однонаправленным итератором.
        \tparam RandomAccessIterator
            Тип итератора выходного диапазона, в который будут записаны отсортированные данные.
            Должен быть итератором произвольного доступа.
        \tparam Map
            Отображение входных значений в целые числа.
            Сортировка происходит по значениям этого отображения. Поэтому от него требуется, чтобы
            результатом отображения были целые числа.

            Алгоритм работы.

        1. Заводится массив счётчиков, по длине не уступающий максимальному значению, принимаемому
           типом сортируемых чисел, поскольку индексы в массиве счётчиков соответствуют сортируемым
           числам.
        2. Входной диапазон при помощи заданного отображения переводится в набор целых чисел, и для
           каждого из них подсчитывается количество вхождений в исходный диапазон.
        3. Массив счётчиков преобразуется в кумулятивный вид, то есть к каждому (i + 1)-му счётчику
           последовательно прибавляется значение i-го. Таким образом получается массив, в котором
           на i-м месте лежит индекс в выходном массиве элемента, который отображается в число i.
        4. Проходим по входному диапазону и, используя полученный массив индексов, записываем
           элементы входного диапазона на их места в отсортированном диапазоне.
     */
    template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_copy
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            detail::counting_sort_impl(first, last, result, compose(to_unsigned, std::move(map)));
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_copy
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result
        )
    {
        return counting_sort_copy(first, last, result, identity);
    }

    template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            detail::counting_sort_impl
            (
                std::make_move_iterator(first),
                std::make_move_iterator(last),
                result,
                compose(to_unsigned, std::move(map))
            );
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result
        )
    {
        return counting_sort_move(first, last, result, identity);
    }

    //!     Диапазонный вариант сортировки подсчётом
    /*!
            Отличается только тем, что вместо пары итераторов принимает диапазон.
     */
    template <typename ForwardRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_copy
        (
            ForwardRange && range,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            counting_sort_copy
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range)),
                result,
                map
            );
    }

    template <typename ForwardRange, typename RandomAccessIterator>
    RandomAccessIterator counting_sort_copy (ForwardRange && range, RandomAccessIterator result)
    {
        return
            counting_sort_copy
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range)),
                result
            );
    }

    template <typename ForwardRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardRange && range,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            counting_sort_move
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range)),
                result,
                map
            );
    }

    template <typename ForwardRange, typename RandomAccessIterator>
    RandomAccessIterator counting_sort_move (ForwardRange && range, RandomAccessIterator result)
    {
        return
            counting_sort_move
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range)),
                result
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_HPP
