#ifndef BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_PAR_HPP
#define BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_PAR_HPP

#include <burst/algorithm/detail/get_shape.hpp>
#include <burst/algorithm/detail/parallel_radix_sort.hpp>
#include <burst/algorithm/detail/radix_sort.hpp>
#include <burst/algorithm/radix_sort/radix_sort_seq.hpp>
#include <burst/execution/parallel_policy.hpp>
#include <burst/functional/compose.hpp>
#include <burst/functional/identity.hpp>
#include <burst/functional/low_byte.hpp>
#include <burst/integer/shift_to_unsigned.hpp>

#include <boost/asio/thread_pool.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Параллельный вариант поразрядной сортировки

        \details
            Пытается создать пул с указанным количеством рабочих потоков и вызвать параллельную
            реализацию поразрядки.
            Если потоков указано слишком мало (меньше двух) или диапазон слишком маленький, чтобы
            сортировать его в указанном количестве потоков, то происходит откат на последовательный
            вариант алгоритма.

        \param par
            Тег, указывающий на то, что нужно вызвать параллельный вариант алгоритма.
            Содержит в себе желаемое количество потоков для параллелизации.

        \see radix_sort_seq.hpp
        \see parallel_policy
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
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer,
            Map map,
            Radix radix
        )
    {
        const auto shape = detail::get_shape(par, first, last);
        const auto thread_count = shape[0];
        if (thread_count > 1)
        {
            boost::asio::thread_pool pool(thread_count);
            detail::radix_sort_impl
            (
                pool,
                shape,
                first,
                last,
                buffer,
                compose(shift_to_unsigned, std::move(map)),
                radix
            );
        }
        else
        {
            radix_sort(first, last, buffer, map, radix);
        }
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    void
        radix_sort
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer,
            Map map
        )
    {
        radix_sort(par, first, last, buffer, map, low_byte);
    }

    template <typename RandomAccessIterator1, typename RandomAccessIterator2>
    void
        radix_sort
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 buffer
        )
    {
        radix_sort(par, first, last, buffer, identity, low_byte);
    }

    /*!
        \brief
            Диапазонный вариант параллельной поразрядной сортировки

        \details
            Сортируемые значения задаются не парой итераторов, а диапазоном. Кроме того, выполняется
            параллельно в несколько потоков.
     */
    template
    <
        typename RandomAccessRange,
        typename RandomAccessIterator,
        typename Map,
        typename Radix
    >
    void
        radix_sort
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator buffer,
            Map map,
            Radix radix
        )
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            par,
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer,
            map,
            radix
        );
    }

    template <typename RandomAccessRange, typename RandomAccessIterator, typename Map>
    void
        radix_sort
        (
            parallel_policy par,
            RandomAccessRange && range,
            RandomAccessIterator buffer,
            Map map
        )
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            par,
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer,
            map
        );
    }

    template <typename RandomAccessRange, typename RandomAccessIterator>
    void radix_sort (parallel_policy par, RandomAccessRange && range, RandomAccessIterator buffer)
    {
        using std::begin;
        using std::end;
        radix_sort
        (
            par,
            begin(std::forward<RandomAccessRange>(range)),
            end(std::forward<RandomAccessRange>(range)),
            buffer
        );
    }
} // namespace burst

#endif // BURST__ALGORITHM__RADIX_SORT__RADIX_SORT_PAR_HPP
