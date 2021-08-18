#ifndef BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP
#define BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP

#include <burst/algorithm/counting_sort_copy_par.hpp>
#include <burst/execution/parallel_policy.hpp>
#include <burst/functional/identity.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Параллельная сортировка подсчётом с переносом элементов из исходного диапазона в
            результирующий

        \details
            Кроме того, что переносит, а не копирует элементы при сортировке, ещё и выполняется
            параллельно в несколько потоков.

        \see counting_sort_move_seq.hpp
        \see counting_sort_copy
        \see parallel_policy
     */
    template <typename RandomAccessIterator1, typename RandomAccessIterator2, typename Map>
    RandomAccessIterator2
        counting_sort_move
        (
            parallel_policy par,
            RandomAccessIterator1 first,
            RandomAccessIterator1 last,
            RandomAccessIterator2 result,
            Map map
        )
    {
        return
            counting_sort_copy
            (
                par,
                std::make_move_iterator(first),
                std::make_move_iterator(last),
                result,
                map
            );
    }

    /*!
        \brief
            Диапазонный вариант параллельной сортировки подсчётом с переносом элементов из исходного
            диапазона в результирующий
     */
    template <typename RandomAccessRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
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
            counting_sort_move
            (
                par,
                begin(std::forward<RandomAccessRange>(range)),
                end(std::forward<RandomAccessRange>(range)),
                result,
                map
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_MOVE_PAR_HPP
