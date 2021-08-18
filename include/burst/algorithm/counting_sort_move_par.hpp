#ifndef BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP
#define BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP

#include <burst/algorithm/counting_sort_copy_par.hpp>
#include <burst/execution/parallel_policy.hpp>
#include <burst/functional/identity.hpp>

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
    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    RandomAccessIterator2
        counting_sort_move
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 result,
            Map map
        )
    {
        return
            counting_sort_copy
            (
                par,
                std::make_move_iterator(first),
                std::make_move_iterator(last),
                result,
                map
            );
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2>
    RandomAccessIterator2
        counting_sort_move
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 result
        )
    {
        return counting_sort_move(par, first, last, result, identity);
    }

    template <typename RandomAccessRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator result,
            Map map
        )
    {
        using std::begin;
        using std::end;
        return
            counting_sort_move
            (
                par,
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                result,
                map
            );
    }

    template <typename RandomAccessRange, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_move
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator result
        )
    {
        using std::begin;
        using std::end;
        return
            counting_sort_move
            (
                par,
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                result
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP
