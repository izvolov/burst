#ifndef BURST_ALGORITHM_SORTING_COUNTING_SORT_COPY_HPP
#define BURST_ALGORITHM_SORTING_COUNTING_SORT_COPY_HPP

#include <iterator>
#include <limits>
#include <type_traits>

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/sorting/detail/counting_sort.hpp>

namespace burst
{
    //!     Сортировка подсчётом без буферизации.
    /*!
            Алгоритм целочисленной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение сортируемых целых чисел.
            Дополнительная память, в которую и будет записан отсортированный диапазон, задаётся
        пользователем в виде выходного итератора. Внутри функции дополнительная память для
        диапазона не создаётся.
            Максимальное значение вычисляется на этапе компиляции, исходя из типа, возвращаемого
        отображением "Map".

        \tparam ForwardIterator
            Тип принимаемого на вход диапазона, который нужно отсортировать. Для него достаточно
            быть однонаправленным итератором.
        \tparam RandomAccessIterator
            Тип итератора, в который будет записан отсортированный диапазон. Должен быть итератором
            произвольного доступа.
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
    void counting_sort_copy (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
    {
        using traits = detail::counting_sort_traits<ForwardIterator, Map>;

        using difference_type = typename std::iterator_traits<RandomAccessIterator>::difference_type;
        // Единица для дополнительного нуля в начале массива.
        difference_type counters[traits::value_range + 1] = {0};

        detail::collect(first, last, map, counters);
        detail::dispose(first, last, result, map, counters);
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    void counting_sort_copy (ForwardIterator first, ForwardIterator last, RandomAccessIterator result)
    {
        return counting_sort_copy(first, last, result, identity<>());
    }
}

#endif // BURST_ALGORITHM_SORTING_COUNTING_SORT_COPY_HPP
