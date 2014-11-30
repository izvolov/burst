#ifndef THRUST_ITERATOR_DETAIL_INVERT_COMPARE_HPP
#define THRUST_ITERATOR_DETAIL_INVERT_COMPARE_HPP

namespace thrust
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

#endif // THRUST_ITERATOR_DETAIL_INVERT_COMPARE_HPP
