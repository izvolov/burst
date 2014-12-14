#ifndef THRUST_ALGORITHM_SORTING_COUNTING_SORT_HPP
#define THRUST_ALGORITHM_SORTING_COUNTING_SORT_HPP

#include <algorithm>
#include <climits>
#include <iterator>
#include <type_traits>

#include <thrust/algorithm/identity.hpp>

namespace thrust
{
    //!     Сортировка подсчётом.
    /*!
            Алгоритм целочисленной сортировки, работающий за линейное время и использующий
        O(max(N, M)) дополнительной памяти, где N — размер входного диапазона, M — максимальное
        значение сортируемых целых чисел.
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
            результатом отображения были целые беззнаковые числа.

            Алгоритм работы.

        1. Заводится массив счётчиков, по длине равный максимальному значению сортируемых чисел (на
           самом деле, не совсем равный, см. реализацию).
        2. Входной диапазон при помощи заданного отображения переводится в набор целых чисел, и для
           каждого из них подсчитывается количество вхождений в исходный диапазон.
        3. Массив счётчиков преобразуется в кумулятивный вид, то есть к каждому (i + 1)-му счётчику
           последовательно прибавляется значение i-го. Таким образом получается массив, в котором
           на i-м месте лежит индекс в выходном массиве элемента, который отображается в число i.
        4. Проходим по входному диапазону и, используя полученный массив индексов, записываем
           элементы входного диапазона на их места в отсортированном диапазоне.
     */
    template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
    void counting_sort (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
    {
        typedef typename std::iterator_traits<ForwardIterator>::value_type preimage_type;
        typedef typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type image_type;
        static_assert
        (
            std::is_integral<image_type>::value && std::is_unsigned<image_type>::value,
            "Сортируемые элементы должны быть отображены в беззнаковые целые числа."
        );

        constexpr const std::size_t bits_in_image = sizeof(image_type) * CHAR_BIT;
        constexpr const std::size_t max_value = (1ul << bits_in_image) - 1;

        // Единица за счёт индексации с нуля и единица для дополнительного нуля в начале массива.
        constexpr const std::size_t counters_size = max_value + 1 + 1;
        std::size_t counters[counters_size] = {0};

        std::for_each(first, last,
            [& counters, & map] (const preimage_type & preimage)
            {
                ++counters[map(preimage) + 1];
            });

        for (std::size_t i = 1; i < counters_size; ++i)
        {
            counters[i] += counters[i - 1];
        }

        std::for_each(first, last,
            [& result, & counters, & map] (const preimage_type & preimage)
            {
                typedef typename std::iterator_traits<RandomAccessIterator>::difference_type difference_type;
                auto index = static_cast<difference_type>(counters[map(preimage)]++);
                result[index] = preimage;
            });
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    void counting_sort (ForwardIterator first, ForwardIterator last, RandomAccessIterator result)
    {
        return counting_sort(first, last, result, identity<>());
    }
}

#endif // THRUST_ALGORITHM_SORTING_COUNTING_SORT_HPP
