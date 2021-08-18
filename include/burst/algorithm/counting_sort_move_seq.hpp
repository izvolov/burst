#ifndef BURST_ALGORITHM_COUNTING_SORT_MOVE_SEQ_HPP
#define BURST_ALGORITHM_COUNTING_SORT_MOVE_SEQ_HPP

#include <burst/algorithm/counting_sort_copy_seq.hpp>
#include <burst/functional/identity.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Сортировка подсчётом с переносом элементов из исходного диапазона в результирующий

        \details
            Отличается от `counting_sort_copy` только тем, что элементы при сортировке не
            копируются в результирующий диапазон, а переносятся.

        \see counting_sort_copy
     */
    template <typename ForwardIterator, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result,
            Map map
        )
    {
        return
            counting_sort_copy
            (
                std::make_move_iterator(first),
                std::make_move_iterator(last),
                result,
                map
            );
    }

    template <typename ForwardIterator, typename RandomAccessIterator>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardIterator first,
            ForwardIterator last,
            RandomAccessIterator result
        )
    {
        return counting_sort_move(first, last, result, identity);
    }

    /*!
        \brief
            Диапазонный вариант сортировки подсчётом с переносом элементов из исходного диапазона в
            результирующий
     */
    template <typename ForwardRange, typename RandomAccessIterator, typename Map>
    RandomAccessIterator
        counting_sort_move
        (
            ForwardRange && range,
            RandomAccessIterator result,
            Map map
        )
    {
        using std::begin;
        using std::end;
        return
            counting_sort_move
            (
                begin(std::forward<ForwardRange>(range)),
                end(std::forward<ForwardRange>(range)),
                result,
                map
            );
    }

    template <typename ForwardRange, typename RandomAccessIterator>
    RandomAccessIterator counting_sort_move (ForwardRange && range, RandomAccessIterator result)
    {
        using std::begin;
        using std::end;
        return
            counting_sort_move
            (
                begin(std::forward<ForwardRange>(range)),
                end(std::forward<ForwardRange>(range)),
                result
            );
    }
}

#endif // BURST_ALGORITHM_COUNTING_SORT_MOVE_SEQ_HPP
