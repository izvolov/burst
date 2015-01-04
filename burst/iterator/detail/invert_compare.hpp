#ifndef BURST_ITERATOR_DETAIL_INVERT_COMPARE_HPP
#define BURST_ITERATOR_DETAIL_INVERT_COMPARE_HPP

namespace burst
{
    namespace detail
    {
        template <typename Compare>
        struct invert_comparison
        {
            invert_comparison (const Compare & compare):
                compare(compare)
            {
            }

            invert_comparison () = default;

            template <typename Object>
            bool operator () (const Object & left, const Object & right) const
            {
                return compare(right, left);
            }

            Compare compare;
        };
    }
}

#endif // BURST_ITERATOR_DETAIL_INVERT_COMPARE_HPP
