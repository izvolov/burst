#ifndef BURST_ALGORITHM_COUNTING_SORT_COPY_PAR_HPP
#define BURST_ALGORITHM_COUNTING_SORT_COPY_PAR_HPP

#include <burst/algorithm/counting_sort_copy_seq.hpp>
#include <burst/algorithm/detail/get_shape.hpp>
#include <burst/algorithm/detail/parallel_counting_sort.hpp>
#include <burst/execution/parallel_policy.hpp>
#include <burst/functional/compose.hpp>
#include <burst/functional/identity.hpp>
#include <burst/integer/to_unsigned.hpp>

#include <boost/asio/thread_pool.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Параллельный вариант сортировки подсчётом

        \details
            Отличия от последовательного алгоритма:

            Так же, как и последовательный алгоритм, работает за время `O(N)`, но использует больше
            дополнительной памяти: `O(max(N, M * T))`, где `N` — размер входного диапазона, `M` —
            максимальное значение, которое может принимать тип сортируемых чисел, `T` — количество
            потоков, участвующих в сортировке.

            Алгоритм работы:
            1.  Разбивает входной диапазон на логические куски, каждый из которых будет
                обрабатываться в отдельном потоке.
            2.  В каждом куске собираются счётчики. У каждого потока свой массив счётчиков,
                независимый от других потоков.
            3.  Когда все потоки собрали свои счётчики, все счётчики синхронизируются между кусками
                так, чтобы каждый поток знал позиции именно своих элементов в итоговом массиве
                (см. Harada, Howes, "Introduction to GPU Radix Sort"
                http://www.heterogeneouscompute.org/wordpress/wp-content/uploads/2011/06/RadixSort.pdf)
            4.  Каждый поток распределяет элементы своего куска по результирующему диапазону.

        \param par
            Тег, указывающий на то, что нужно вызвать параллельный вариант алгоритма, и содержащий
            желаемое количество потоков для параллелизации.
        \param [first, last)
            Диапазон, который нужно отсортировать. Должен быть диапазоном произвольного доступа.

        \see counting_sort_copy_seq.hpp
        \see parallel_policy
     */
    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    RandomAccessIterator2
        counting_sort_copy
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 result,
            Map map
        )
    {
        const auto shape = detail::get_shape(par, first, last);
        const auto thread_count = shape[0];
        if (thread_count > 1)
        {
            boost::asio::thread_pool pool(thread_count);
            return
                detail::counting_sort_impl
                (
                    pool,
                    shape,
                    first,
                    last,
                    result,
                    compose(to_unsigned, std::move(map))
                );
        }
        else
        {
            return counting_sort_copy(first, last, result, map);
        }
    }

    /*!
        \brief
            Параллельная сортировка подсчётом без пользовательского отображения

        \details
            В этом случае элементы берутся "как есть" то есть подсчитываются непосредственно
            значения элементов, лежащих во входном диапазоне.
     */
    template <typename RandomAccessIterator1, typename RandomAccessIterator2>
    RandomAccessIterator2
        counting_sort_copy
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 result
        )
    {
        return counting_sort_copy(par, first, last, result, identity);
    }

    /*!
        \brief
            Диапазонный вариант параллельной сортировки подсчётом

        \details
            Сортируемые данные задаются не парой итераторов, а сразу диапазоном.
     */
    template <typename RandomAccessRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_copy
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator result,
            Map map
        )
    {
        using std::begin;
        using std::end;
        return
            counting_sort_copy
            (
                par,
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                result,
                map
            );
    }

    /*!
        \brief
            Диапазонный вариант параллельной сортировки подсчётом без пользовательского отображения

        \details
            Сортируемые данные задаются не парой итераторов, а сразу диапазоном, а элементы берутся
            "как есть" то есть подсчитываются непосредственно значения элементов, лежащих во входном
            диапазоне.
     */
    template <typename RandomAccessRange, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_copy
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator result
        )
    {
        using std::begin;
        using std::end;
        return
            counting_sort_copy
            (
                par,
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                result
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_COPY_PAR_HPP
