#ifndef BURST_RANGE_SKIP_TO_LOWER_BOUND_HPP
#define BURST_RANGE_SKIP_TO_LOWER_BOUND_HPP

#include <burst/range/detail/skip_to_lower_bound.hpp>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>

#include <functional>

namespace burst
{
    //!     "Прокрутить" диапазон до нижней границы относительно предиката.
    /*!
            Принимает на вход диапазон ("range"), упорядоченный относительно заданного отношения
        порядка ("compare"), а также целевой элемент ("goal").
            Продвигает начало диапазона до первого элемента этого диапазона, который не меньше
        целевого элемента.
            Если в диапазоне такого элемента нет, то диапазон становится пустым.
     */
    template <typename Range, typename Value, typename Compare>
    void skip_to_lower_bound (Range & range, const Value & goal, Compare compare)
    {
        BOOST_ASSERT(boost::algorithm::is_sorted(range, compare));
        detail::skip_to_lower_bound(range, goal, compare);
    }

    //!     "Прокрутить" диапазон до нижней границы.
    /*!
            Работает аналогично "прокрутке" с предикатом, только отношение порядка выбиарает
        по-умолчанию — в данном случае это std::less.
     */
    template <typename Range, typename Value>
    void skip_to_lower_bound (Range & range, const Value & goal)
    {
        detail::skip_to_lower_bound(range, goal, std::less<>{});
    }

}

#endif // BURST_RANGE_SKIP_TO_LOWER_BOUND_HPP
