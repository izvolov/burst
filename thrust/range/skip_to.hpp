#ifndef THRUST_RANGE_SKIP_TO_HPP
#define THRUST_RANGE_SKIP_TO_HPP

#include <boost/range/algorithm/lower_bound.hpp>

namespace thrust
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
        auto new_begin = boost::lower_bound(range, goal, compare);
        range = boost::make_iterator_range(new_begin, range.end());
    }

    //!     "Прокрутить" диапазон до нужного значения.
    /*!
            Работает аналогично "прокрутке" с предикатом, только отношение порядка выбиарает
        по-умолчанию — в данном случае это std::less.
     */
    template <typename Range, typename Value>
    void skip_to (Range & range, const Value & goal)
    {
        skip_to(range, goal, std::less<Value>());
    }
}

#endif // THRUST_RANGE_SKIP_TO_HPP
