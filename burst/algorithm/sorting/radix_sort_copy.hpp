#ifndef BURST_ALGORITHM_SORTING_RADIX_SORT_COPY_HPP
#define BURST_ALGORITHM_SORTING_RADIX_SORT_COPY_HPP

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/low_byte.hpp>
#include <burst/algorithm/sorting/detail/radix_sort.hpp>

namespace burst
{
    //!     Поразрядная сортировка с выводом.
    /*!
            Алгоритм поразрядной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение одного разряда сортируемых целых чисел.
            Отсортированные данные будут записаны в выходной диапазон. Входной диапазон не
        изменяется.
            Максимальное значение вычисляется на этапе компиляции, исходя из типа, возвращаемого
        отображением "Radix".

        \tparam ForwardIterator
            Тип принимаемого на вход диапазона, который нужно отсортировать. Для него достаточно
            быть однонаправленным итератором.
        \tparam Iterator
            Тип итератора выходного диапазона, в который будут записаны сортированные данные.

            1. Однонаправленный итератор.
               Если итератор однонаправленный и разрядов в сортируемых числах больше одного, то для
               сортировки будет использовано два дополнительных буфера, а по окончании
               сортированные данные будут перемещены в выходной диапазон.
               Если в сортируемых числах один разряд, то будет использован только один буфер.

            2. Итератор произвольного доступа.
               Если итератор произвольного доступа и в сортируемых числах больше одного разряда, то
               для сортировки будет заведён только один дополнительный буфер, а в роли второго
               буфера будет выступать выходной диапазон.
               Если в сортируемых числах один разряд, то дополнительного буфера не потребуется.
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
        \return Iterator
            Возвращает итератор за последним отсортированным элементом в выходном диапазоне.

            Алгоритм работы.

            Алгоритм эквивалентен последовательному вызову сортировки подсчётом для каждого разряда
        сортируемых чисел от младшего разряда к старшему. Но детали реализации несколько другие. В
        частности, счётчики для сортировки подсчётом вычисляются за один проход для всех разрядов,
        а не отдельным проходом на каждый разряд.
     */
    template <typename ForwardIterator, typename Iterator, typename Map, typename Radix>
    Iterator radix_sort_copy (ForwardIterator first, ForwardIterator last, Iterator result, Map map, Radix radix)
    {
        return detail::radix_sort_copy_impl(first, last, result, map, radix);
    }

    template <typename ForwardIterator, typename Iterator, typename Map>
    Iterator radix_sort_copy (ForwardIterator first, ForwardIterator last, Iterator result, Map map)
    {
        return radix_sort_copy(first, last, result, map, low_byte<>());
    }

    template <typename ForwardIterator, typename Iterator>
    Iterator radix_sort_copy (ForwardIterator first, ForwardIterator last, Iterator result)
    {
        return radix_sort_copy(first, last, result, identity<>(), low_byte<>());
    }
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_RADIX_SORT_COPY_HPP
