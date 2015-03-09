#ifndef BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP
#define BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP

#include <algorithm>
#include <iterator>
#include <vector>

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/sorting/counting_sort_copy.hpp>

namespace burst
{
    //!     Сортировка подсчётом с буферизацией.
    /*!
            Алгоритм целочисленной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение сортируемых целых чисел.
            В процессе сортировки используется внутренний буфер, в который записывается
        отсортированный диапазон, а затем его элементы перемещаются обратно во входной диапазон.
            Максимальное значение вычисляется на этапе компиляции, исходя из типа, возвращаемого
        отображением "Map".

        \tparam ForwardIterator
            Тип принимаемого на вход диапазона, который нужно отсортировать. Для него достаточно
            быть однонаправленным итератором.
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
    template <typename ForwardIterator, typename Map>
    void counting_sort (ForwardIterator first, ForwardIterator last, Map map)
    {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        std::vector<value_type> buffer(static_cast<std::size_t>(std::distance(first, last)));

        counting_sort_copy(first, last, buffer.begin(), map);
        std::move(buffer.begin(), buffer.end(), first);
    }

    template <typename ForwardIterator>
    void counting_sort (ForwardIterator first, ForwardIterator last)
    {
        return counting_sort(first, last, identity<>());
    }
}

#endif // BURST_ALGORITHM_SORTING_COUNTING_SORT_HPP
