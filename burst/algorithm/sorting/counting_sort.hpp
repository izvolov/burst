#ifndef BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP
#define BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP

#include <burst/algorithm/sorting/detail/counting_sort.hpp>
#include <burst/functional/identity.hpp>

namespace burst
{
    //!     Сортировка подсчётом.
    /*!
            Алгоритм целочисленной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение сортируемых целых чисел. Максимальное значение вычисляется на этапе компиляции,
        исходя из типа, возвращаемого отображением "Map".
            Результат сортировки записывается в выходной диапазон, а входной диапазон остаётся без
        изменений. Выходной диапазон должен быть не меньше входного, чтобы в него влезли
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
    RandomAccessIterator counting_sort (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
    {
        return detail::counting_sort_impl(first, last, result, map);
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    RandomAccessIterator counting_sort (ForwardIterator first, ForwardIterator last, RandomAccessIterator result)
    {
        return counting_sort(first, last, result, identity<>());
    }
}

#endif // BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP
