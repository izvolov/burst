#ifndef BURST_ALGORITHM_SORTING_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_RADIX_SORT_HPP

#include <climits>
#include <iterator>
#include <type_traits>

#include <burst/algorithm/identity.hpp>
#include <burst/algorithm/low_byte.hpp>
#include <burst/algorithm/sorting/detail/counting_sort.hpp>
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
        using traits = detail::radix_sort_traits<ForwardIterator, Map, Radix>;

        using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
        difference_type counters[traits::radix_count][traits::radix_value_range + 1] = {{0}};
        detail::collect(first, last, map, radix, counters, std::make_index_sequence<traits::radix_count>());

        auto distance = std::distance(first, last);
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        std::vector<value_type> resulting_buffer(static_cast<std::size_t>(distance));
        std::vector<value_type> intermediate_buffer(static_cast<std::size_t>(distance));

        auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
        detail::dispose(first, last, resulting_buffer.begin(), get_low_radix, counters[0]);

        for (std::size_t radix_number = 1; radix_number < traits::radix_count; ++radix_number)
        {
            auto get_radix =
                [& map, & radix, & radix_number] (const value_type & value)
                {
                    return radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * radix_number)));
                };
            detail::dispose(resulting_buffer.begin(), resulting_buffer.end(),
                intermediate_buffer.begin(),
                get_radix,
                counters[radix_number]);

            std::swap(resulting_buffer, intermediate_buffer);
        }

        std::move(resulting_buffer.begin(), resulting_buffer.end(), first);
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
