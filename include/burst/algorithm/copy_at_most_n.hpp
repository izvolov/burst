#ifndef BURST_ALGORITHM_COPY_AT_MOST_N_HPP
#define BURST_ALGORITHM_COPY_AT_MOST_N_HPP

#include <burst/algorithm/detail/copy_at_most_n.hpp>
#include <burst/type_traits/iterator_category.hpp>

#include <initializer_list>
#include <iterator>
#include <tuple>

namespace burst
{
    /*!
        \brief
            Копировать не более n элементов последовательности

        \details
            Копирует в выходной итератор `n` элементов входной последовательности, если
            `distance(first, last) >= n`, либо всю последовательность в противном случае.

        \param [first, last)
            Последовательность, элементы которой будут скопированы.
        \param n
            Количество элементов, которое требуется скопировать.
        \param result
            Итератор, в который будут записаны скопированные элементы.

        \returns
            Тройку `(p, m, q)`, где `p` — позиция за последним скопированным элементом в исходном
            диапазоне, `m` — количество скопированных элементов, `q` — позиция за последним
            скопированным элементом в результирующем диапазоне.
     */
    template <typename InputIterator, typename Integral, typename OutputIterator>
    std::tuple<InputIterator, Integral, OutputIterator>
        copy_at_most_n (InputIterator first, InputIterator last, Integral n, OutputIterator result)
    {
        using iterator_category = iterator_category_t<InputIterator>;
        return detail::copy_at_most_n_impl(first, last, n, result, iterator_category{});
    }

    /*!
        \brief
            Перегрузка для работы с диапазонами

        \returns
            `copy_at_most_n(begin(r), end(r), n, result);`

        \see copy_at_most_n
     */
    template <typename InputRange, typename Integral, typename OutputIterator>
    auto copy_at_most_n (InputRange && r, Integral n, OutputIterator result)
    {
        using std::begin;
        using std::end;
        return copy_at_most_n(begin(r), end(r), n, result);
    }

    /*!
        \brief
            Перегрузка для работы со списком инициализации

        \returns
            `copy_at_most_n(l.begin(), l.end(), n, result);`

        \see copy_at_most_n
     */
    template <typename Value, typename Integral, typename OutputIterator>
    auto copy_at_most_n (std::initializer_list<Value> l, Integral n, OutputIterator result)
    {
        return copy_at_most_n(l.begin(), l.end(), n, result);
    }
} // namespace burst

#endif // BURST_ALGORITHM_COPY_AT_MOST_N_HPP
