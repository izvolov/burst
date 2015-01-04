#ifndef BURST_RANGE_SKIP_TO_HPP
#define BURST_RANGE_SKIP_TO_HPP

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>

#include <burst/range/detail/skip_to.hpp>

namespace burst
{
    //!     "Прокрутить" диапазон до нужного значения относительно предиката.
    /*!
            Принимает на вход диапазон ("range"), упорядоченный относительно заданного отношения
        порядка ("compare"), а также целевой элемент ("goal").
            Продвигает диапазон таким образом, что он либо заканчивается, либо первый его элемент
        становится не меньше целевого элемента.
     */
    template <typename Range, typename Value, typename Compare>
    void skip_to (Range & range, const Value & goal, Compare compare)
    {
        BOOST_ASSERT(boost::algorithm::is_sorted(range, compare));
        detail::skip_to(range, goal, compare);
    }

    //!     "Прокрутить" диапазон до нужного значения.
    /*!
            Работает аналогично "прокрутке" с предикатом, только отношение порядка выбиарает
        по-умолчанию — в данном случае это std::less.
     */
    template <typename Range, typename Value>
    void skip_to (Range & range, const Value & goal)
    {
        detail::skip_to(range, goal, std::less<Value>());
    }

}

#endif // BURST_RANGE_SKIP_TO_HPP
