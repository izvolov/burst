#ifndef THRUST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP
#define THRUST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP

namespace thrust
{
    namespace detail
    {
        template <typename Compare>
        struct front_value_comparator
        {
            front_value_comparator (const Compare & compare):
                compare(compare)
            {
            }

            front_value_comparator () = default;

            template <typename Range>
            bool operator () (const Range & left, const Range & right) const
            {
                return compare(left.front(), right.front());
            }

            Compare compare;
        };

        template <typename Compare>
        front_value_comparator<Compare> compare_by_front_value (const Compare & compare)
        {
            return front_value_comparator<Compare>(compare);
        }
    } // namespace detail
} // namespace thrust

#endif // THRUST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP
