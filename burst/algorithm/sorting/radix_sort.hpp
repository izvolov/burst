#ifndef BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_RADIX_SORT_HPP

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/low_byte.hpp>
#include <burst/algorithm/sorting/detail/radix_sort.hpp>

namespace burst
{
    //!     Поразрядная сортировка.
    /*!
            Алгоритм поразрядной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение одного разряда сортируемых целых чисел.
            Максимальное значение вычисляется на этапе компиляции, исходя из типа, возвращаемого
        отображением "Radix".

        \tparam ForwardIterator
            Тип принимаемого на вход диапазона, который нужно отсортировать. Для него достаточно
            быть однонаправленным итератором.
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
    template <typename ForwardIterator, typename Map, typename Radix>
    void radix_sort (ForwardIterator first, ForwardIterator last, Map map, Radix radix)
    {
        detail::radix_sort_impl(first, last, map, radix);
    }

    template <typename ForwardIterator, typename Map>
    void radix_sort (ForwardIterator first, ForwardIterator last, Map map)
    {
        radix_sort(first, last, map, low_byte<>());
    }

    template <typename ForwardIterator>
    void radix_sort (ForwardIterator first, ForwardIterator last)
    {
        radix_sort(first, last, identity<>(), low_byte<>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
