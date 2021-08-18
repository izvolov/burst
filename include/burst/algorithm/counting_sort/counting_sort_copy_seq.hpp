#ifndef BURST_ALGORITHM_COUNTING_SORT_COPY_SEQ_HPP
#define BURST_ALGORITHM_COUNTING_SORT_COPY_SEQ_HPP

#include <burst/algorithm/detail/counting_sort.hpp>
#include <burst/functional/compose.hpp>
#include <burst/functional/identity.hpp>
#include <burst/integer/to_unsigned.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Классическая устойчивая сортировка подсчётом

        \details
            [Устойчивый алгоритм сортировки подсчётом](https://ru.wikipedia.org/wiki/Сортировка_подсчётом#Устойчивый_алгоритм)

            Алгоритм целочисленной сортировки, работающий за время `O(N)` и использующий
            `O(max(N, M))` дополнительной памяти, где `N = |[first, last)|` — размер входного
            диапазона, `M = std::numeric_limits<decltype(map(*first))>::max()` — максимальное
            значение, которое может принимать тип сортируемых чисел.

            В версии "_copy" результат сортировки копируется в выходной диапазон, а входной диапазон
            остаётся без изменений.

            Алгоритм работы:
            1.  Заводится массив счётчиков размера `M`.
            2.  Входной диапазон при помощи отображения `map` переводится в набор целых чисел, и для
                каждого из этих чисел подсчитывается количество вхождений.
            3.  Массив счётчиков преобразуется в кумулятивный вид, то есть к каждому `(i + 1)`-му
                счётчику последовательно прибавляется значение `i`-го. Таким образом получается
                массив, в котором на `i`-м месте лежит индекс в выходном массиве элемента, который
                отображается в число `i`.
            4.  Используя полученный массив индексов, записываем элементы входного диапазона на их
                места в отсортированном диапазоне.

        \param [first, last)
            Диапазон, который нужно отсортировать. Должен быть хотя бы однонаправленным диапазоном.
        \param result
            Итератор на начало выходного диапазона, в который будут записаны отсортированные данные.
            Выходной диапазон, начало которого задаёт этот итератор, должен быть достаточного
            размера (не меньше, чем `|[first, last)|`), чтобы в него был записан отсортированный
            массив.
            Должен быть итератором произвольного доступа.
        \param map
            Отображение входных значений в целые числа.
            Сортировка происходит по значениям этого отображения. Поэтому от него требуется, чтобы
            результатом отображения были целые числа, причём лучшие результаты достигаются, как
            правило, на однобайтовых числах.

        \returns
            Итератор за последним отсортированным элементом в выходном диапазоне.
     */
    template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_copy
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            detail::counting_sort_impl(first, last, result, compose(to_unsigned, std::move(map)));
    }

    /*!
        \brief
            Перегрузка без пользовательского отображения

        \details
            В этом случае элементы берутся "как есть" то есть подсчитываются непосредственно
            значения элементов, лежащих во входном диапазоне.
     */
    template <typename ForwardIterator, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_copy
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result
        )
    {
        return counting_sort_copy(first, last, result, identity);
    }

    /*!
        \brief
            Диапазонный вариант сортировки подсчётом

        \details
            Отличается только тем, что вместо пары итераторов принимает диапазон.
     */
    template <typename ForwardRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_copy (ForwardRange && range, RandomAccessIterator result, Map map)
    {
        using std::begin;
        using std::end;
        return
            counting_sort_copy
            (
                begin(std::forward<ForwardRange>(range)),
                end(std::forward<ForwardRange>(range)),
                result,
                map
            );
    }

    /*!
        \brief
            Диапазонный вариант сортировки подсчётом без пользовательского отображения

        \details
            Сортируемые данные задаются не парой итераторов, а сразу диапазоном, а элементы берутся
            "как есть" то есть подсчитываются непосредственно значения элементов, лежащих во входном
            диапазоне.
     */
    template <typename ForwardRange, typename RandomAccessIterator>
    RandomAccessIterator counting_sort_copy (ForwardRange && range, RandomAccessIterator result)
    {
        using std::begin;
        using std::end;
        return
            counting_sort_copy
            (
                begin(std::forward<ForwardRange>(range)),
                end(std::forward<ForwardRange>(range)),
                result
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_COPY_SEQ_HPP
