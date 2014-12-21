#ifndef THRUST_ALGORITHM_SORTING_COUNTING_SORT_HPP
#define THRUST_ALGORITHM_SORTING_COUNTING_SORT_HPP

#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

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
    void counting_sort (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map)
    {
        typedef typename std::iterator_traits<ForwardIterator>::value_type preimage_type;
        typedef typename std::decay<typename std::result_of<Map(preimage_type)>::type>::type image_type;
        static_assert(std::is_integral<image_type>::value, "Сортируемые элементы должны быть отображены в целые числа.");
        typedef typename std::iterator_traits<RandomAccessIterator>::difference_type difference_type;

        constexpr const auto min_value = std::numeric_limits<image_type>::min();
        constexpr const auto max_value = std::numeric_limits<image_type>::max();
        constexpr const auto value_range = max_value - min_value + 1;

        // Единица для дополнительного нуля в начале массива.
        difference_type counters[value_range + 1] = {0};

        std::for_each(first, last,
            [& counters, & map] (const preimage_type & preimage)
            {
                ++counters[map(preimage) - min_value + 1];
            });

        std::partial_sum(boost::begin(counters), boost::end(counters), boost::begin(counters));

        std::for_each(first, last,
            [& result, & counters, & map] (const preimage_type & preimage)
            {
                auto index = counters[map(preimage) - min_value]++;
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
