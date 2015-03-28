#ifndef BURST_RANGE_SKIP_TO_UPPER_BOUND_HPP
#define BURST_RANGE_SKIP_TO_UPPER_BOUND_HPP

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>

#include <burst/range/detail/skip_to_upper_bound.hpp>

namespace burst
{
    //!     "Прокрутить" диапазон до верхней границы относительно предиката.
    /*!
            Принимает на вход диапазон ("range"), упорядоченный относительно заданного отношения
        порядка ("compare"), а также целевой элемент ("goal").
            Продвигает начало диапазона до первого элемента этого диапазона, который строго больше
        целевого элемента.
            Если в диапазоне такого элемента нет, то диапазон становится пустым.
     */
    template <typename Range, typename Value, typename Compare>
    void skip_to_upper_bound (Range & range, const Value & goal, Compare compare)
    {
        BOOST_ASSERT(boost::algorithm::is_sorted(range, compare));
        detail::skip_to_upper_bound(range, goal, compare);
    }

    //!     "Прокрутить" диапазон до верхней границы.
    /*!
            Работает аналогично "прокрутке" с предикатом, только отношение порядка выбиарает
        по-умолчанию — в данном случае это std::less.
     */
    template <typename Range, typename Value>
    void skip_to_upper_bound (Range & range, const Value & goal)
    {
        detail::skip_to_upper_bound(range, goal, std::less<Value>());
    }

}

#endif // BURST_RANGE_SKIP_TO_UPPER_BOUND_HPP
