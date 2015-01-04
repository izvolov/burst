#ifndef BURST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP
#define BURST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP

namespace burst
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
} // namespace burst

#endif // BURST_ITERATOR_DETAIL_FRONT_VALUE_COMPARE_HPP
