#ifndef BURST__ALGORITHM__PARTIAL_SUM_MAX_HPP
#define BURST__ALGORITHM__PARTIAL_SUM_MAX_HPP

#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/range_value.hpp>

#include <functional>
#include <iterator>
#include <utility>

namespace burst
{
    /*!
        \brief
            Подсчёт частичных сумм и поиск максимума по суммируемым элементам

        \details
            По смыслу алгоритм эквивалентен следующей последовательности вызовов:

            \code{.cpp}
            auto max = std::max_element(first, last);
            auto result = std::partial_sum(first, last, result);
            \endcode

            Отличия:
            -   Работает строго за один проход;
            -   Возвращает не итератор на максимальный элемент, а само значение максимума.

            Асимптотика:
            -   Время: `O(N), N = |[first, last)|` — размер диапазона.
            -   Память: `O(1)`.

        \param [first, last)
            Диапазон, который будет суммироваться, и в котором будет найден максимум.
            Должен быть хотя бы однопроходным итератором.
        \param result
            Итератор на начало выходного диапазона, в который будет происходить запись частичных
            сумм. Длина выходного диапазона должна быть не менее `|[first, last)|`.
        \param sum
            Функция, которая задаёт правило суммирования значений.
        \param less
            Функция, которая задаёт порядок на элементах входного диапазона (для поиска максимума).

        \returns
            Пару `(i, m)`, где `i` — итератор на элемент за последней частичной суммой в выходном
            диапазоне, `m` — первое из эквивалентных максимальных значений в изначальном диапазоне.
     */
    template
    <
        typename InputIterator,
        typename OutputIterator,
        typename BinaryFunction,
        typename BinaryPredicate
    >
    std::pair<OutputIterator, iterator_value_t<InputIterator>>
        partial_sum_max
        (
            InputIterator first,
            InputIterator last,
            OutputIterator result,
            BinaryFunction plus,
            BinaryPredicate less
        )
    {
        if (first != last)
        {
            auto sum = *first;
            auto max = sum;

            *result = sum;

            while (++first != last)
            {
                auto current = *first;
                if (less(max, current))
                {
                    max = current;
                }

                auto new_sum = plus(std::move(sum), std::move(current));
                sum = std::move(new_sum);

                ++result;
                *result = sum;
            }

            return {++result, max};
        }

        return {result, iterator_value_t<InputIterator>{}};
    }

    template <typename InputIterator, typename OutputIterator, typename BinaryFunction>
    auto
        partial_sum_max
        (
            InputIterator first,
            InputIterator last,
            OutputIterator result,
            BinaryFunction sum
        )
    {
        return partial_sum_max(first, last, result, std::move(sum), std::less<>{});
    }

    template <typename InputIterator, typename OutputIterator>
    auto partial_sum_max (InputIterator first, InputIterator last, OutputIterator result)
    {
        return partial_sum_max(first, last, result, std::plus<>{});
    }

    template
    <
        typename InputRange,
        typename OutputIterator,
        typename BinaryFunction,
        typename BinaryPredicate
    >
    auto
        partial_sum_max
        (
            InputRange && range,
            OutputIterator result,
            BinaryFunction plus,
            BinaryPredicate less
        )
    {
        using std::begin;
        using std::end;
        return partial_sum_max(begin(range), end(range), result, std::move(plus), std::move(less));
    }

    template <typename InputRange, typename OutputIterator, typename BinaryFunction>
    auto partial_sum_max (InputRange && range, OutputIterator result, BinaryFunction plus)
    {
        return
            partial_sum_max
            (
                std::forward<InputRange>(range),
                result,
                std::move(plus),
                std::less<>{}
            );
    }

    template <typename InputRange, typename OutputIterator>
    auto partial_sum_max (InputRange && range, OutputIterator result)
    {
        return partial_sum_max(std::forward<InputRange>(range), result, std::plus<>{});
    }
} // namespace burst

#endif // BURST__ALGORITHM__PARTIAL_SUM_MAX_HPP
