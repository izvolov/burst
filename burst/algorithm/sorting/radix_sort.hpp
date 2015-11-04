#ifndef BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_RADIX_SORT_HPP

#include <burst/algorithm/sorting/detail/radix_sort.hpp>
#include <burst/functional/identity.hpp>
#include <burst/functional/low_byte.hpp>

namespace burst
{
    //!     Поразрядная сортировка.
    /*!
            Алгоритм поразрядной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение одного разряда сортируемых целых чисел. Максимальное значение вычисляется на этапе
        компиляции, исходя из типа, возвращаемого отображением "Radix".
            В процессе сортировки используется дополнительный буфер, размер которого должен быть не
        меньше размера сортируемого диапазона.

        \tparam RandomAccessIterator1
            Тип принимаемого на вход диапазона, который нужно отсортировать. Должен быть итератором
            произвольного доступа.
        \tparam RandomAccessIterator2
            Тип буфера который будет использоваться при сортировке.
        \tparam Map
            Отображение входных объектов в целые числа.
            Сортировка происходит по разрядам числа, полученного из этого отображения. Поэтому от
            него требуется, чтобы результатом отображения были целые числа.
        \tparam Radix
            Функция выделения разряда из целого числа. По значениям этого отображения и происходит
            сортировка.
            Должна возвращать целое число, представляющее младший разряд сортируемого числа.
            Для получения более старших разрядов сортируемое число, полученное отображением Map,
            побитово сдвигается на число битов в разряде, и к нему снова применяется функция Radix.

            Алгоритм работы.

            Алгоритм эквивалентен последовательному вызову сортировки подсчётом для каждого разряда
        сортируемых чисел от младшего разряда к старшему. Но детали реализации несколько другие. В
        частности, счётчики для сортировки подсчётом вычисляются за один проход для всех разрядов,
        а не отдельным проходом на каждый разряд.
     */
    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map, typename Radix>
    void radix_sort (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer, Map map, Radix radix)
    {
        detail::radix_sort_impl(first, last, buffer, map, radix);
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    void radix_sort (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer, Map map)
    {
        radix_sort(first, last, buffer, map, low_byte<>());
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2>
    void radix_sort (RandomAccessIterator1 first, RandomAccessIterator1 last, RandomAccessIterator2 buffer)
    {
        radix_sort(first, last, buffer, identity<>(), low_byte<>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
