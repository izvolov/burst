#ifndef BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_SEQ_HPP
#define BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_SEQ_HPP

#include <burst/algorithm/detail/radix_sort.hpp>
#include <burst/functional/compose.hpp>
#include <burst/functional/identity.hpp>
#include <burst/functional/low_byte.hpp>
#include <burst/integer/to_unsigned.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \~russian
            \brief
                Поразрядная сортировка

            \details
                Алгоритм поразрядной сортировки, работающий за линейное время и использующий
                `O(max(N, M))` дополнительной памяти, где `N` — размер входного диапазона, `M` —
                максимальное значение одного разряда сортируемых целых чисел. Максимальное значение
                вычисляется на этапе компиляции исходя из типа, возвращаемого отображением `radix`.

                Алгоритм работы

                Алгоритм эквивалентен последовательному вызову сортировки подсчётом для каждого
                разряда сортируемых чисел от младшего разряда к старшему. Но детали реализации
                несколько другие. В частности, счётчики для сортировки подсчётом вычисляются за
                один проход для всех разрядов, а не отдельным проходом на каждый разряд.

            \param first
                Итератор на начало сортируемого диапазона.
                Должен быть итератором произвольного доступа.
            \param last
                Итератор конца сортируемого диапазона.
            \param buffer
                Итератор на начало буфера, который будет использоваться в качестве дополнительной
                памяти при сортировке. Размер буфера должен быть не меньше размера сортируемого
                диапазона.
                Должен быть итератором произвольного доступа.
            \param map
                Отображение входных объектов в целые числа.
                Сортировка происходит по разрядам числа, полученного из этого отображения. Поэтому
                от него требуется, чтобы результатом отображения были целые числа.
            \param radix
                Функция выделения разряда из целого числа. По значениям этого отображения и
                происходит сортировка.
                Должна возвращать целое число, представляющее младший разряд сортируемого числа.
                Для получения более старших разрядов сортируемое число, полученное отображением
                `map`, побитово сдвигается на число битов в разряде, и к нему снова применяется
                функция `radix`.

        \~english
            \brief
                Radix Sort

            \details
                The radix sorting algorithm, running in linear time and using `O(max(N, M))`
                additional memory, where `N` is size of an input range, `M` — maximum value of
                the radix of a sorted integer type. The maximum value is calculated at compile time
                based on the type returned by the function `radix`.

                Algorithm

                The algorithm is equivalent to several consecutive calls of counting sort for each
                radix of the sorted numbers from low to high byte. But the implementation details
                are slightly different. For example, the counters for the counting sort are
                calculated in one pass for all radices.

            \param first
                Beginning of the range to be sorted.
                Must meet the requirements of RandomAccessIterator.
            \param last
                End of the range to be sorted.
            \param buffer
                An iterator to the beginning of the buffer to be used as additional memory for
                sorting. The buffer size must be at least the size of the range to be sorted.
                Must meet the requirements of RandomAccessIterator.
            \param map
                The mapping from elements of the input range to integers.
                Sorting is driven by radices of an integer received from this function. So it
                requires that the result of the funcion is an integral number.
            \param radix
                Function extracting one radix of the integer. Values of this function drive the
                sort.
                Must return an integer representing the low radix of the sorted integer.
                Higher radices of the integer are calculating by a bitwise shift of the result of
                function `map` and the `radix` function applied to it.
     */
    template
    <
        typename RandomAccessIterator1,
        typename RandomAccessIterator2,
        typename Map,
        typename Radix
    >
    void
        radix_sort
        (
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer,
            Map map,
            Radix radix
        )
    {
        detail::radix_sort_impl(first, last, buffer, compose(to_unsigned, std::move(map)), radix);
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    void
        radix_sort
        (
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer,
            Map map
        )
    {
        radix_sort(first, last, buffer, map, low_byte);
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2>
    void
        radix_sort
        (
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer
        )
    {
        radix_sort(first, last, buffer, identity, low_byte);
    }

    /*!
        \brief
            Диапазонный вариант поразрядной сортировки

        \details
            Отличается только тем, что сортируемые значения задаются не парой итераторов, а
            диапазоном.
     */
    template
    <
        typename RandomAccessRange,
        typename RandomAccessIterator,
        typename Map,
        typename Radix
    >
    void radix_sort (RandomAccessRange && range, RandomAccessIterator buffer, Map map, Radix radix)
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer,
            map,
            radix
        );
    }

    template <typename RandomAccessRange, typename RandomAccessIterator, typename Map>
    void radix_sort (RandomAccessRange && range, RandomAccessIterator buffer, Map map)
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer,
            map
        );
    }

    template <typename RandomAccessRange, typename RandomAccessIterator>
    void radix_sort (RandomAccessRange && range, RandomAccessIterator buffer)
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer
        );
    }
} // namespace burst

#endif // BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_SEQ_HPP
