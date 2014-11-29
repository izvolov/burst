#ifndef THRUST_ITERATOR_INTERSECT_ITERATOR_HPP
#define THRUST_ITERATOR_INTERSECT_ITERATOR_HPP

#include <algorithm>
#include <vector>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <thrust/iterator/detail/front_value_compare.hpp>
#include <thrust/iterator/end_tag.hpp>
#include <thrust/range/skip_to.hpp>

namespace thrust
{
    template
    <
        typename InputRange,
        typename Compare = std::less<typename InputRange::value_type>
    >
    class intersect_iterator: public boost::iterator_facade
                                 <
                                     intersect_iterator<InputRange, Compare>,
                                     typename InputRange::value_type,
                                     boost::forward_traversal_tag,
                                     const typename InputRange::value_type &
                                 >
    {
    private:
        typedef InputRange range_type;
        typedef Compare compare_type;

        typedef boost::iterator_facade
        <
            intersect_iterator,
            typename range_type::value_type,
            boost::forward_traversal_tag,
            const typename range_type::value_type &
        >
        base_type;

    public:
        template <typename InputRange1>
        explicit intersect_iterator (const InputRange1 & ranges, Compare compare = Compare()):
            m_ranges(),
            m_compare(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename InputRange1::value_type, range_type>::value);

            if (boost::algorithm::none_of(ranges, boost::bind(&range_type::empty, _1)))
            {
                BOOST_ASSERT(boost::algorithm::all_of(ranges.begin(), ranges.end(), boost::bind(&boost::algorithm::is_sorted<range_type, Compare>, _1, compare)));
                m_ranges.assign(ranges.begin(), ranges.end());
                boost::sort(m_ranges, detail::compare_by_front_value(m_compare));
                scroll_to_next_intersection();
            }
        }

        intersect_iterator ():
            m_ranges(),
            m_compare(Compare())
        {
        }

    public:
        void increment ()
        {
            m_ranges.back().advance_begin(1);
            if (not m_ranges.back().empty())
            {
                scroll_to_next_intersection();
            }
            else
            {
                scroll_to_end();
            }
        }

    private:
        void scroll_to_next_intersection ()
        {
            while (m_compare(m_ranges.front().front(), m_ranges.back().front()))
            {
                skip_to(m_ranges.front(), m_ranges.back().front(), m_compare);
                if (not m_ranges.front().empty())
                {
                    auto second_range = ++m_ranges.begin();
                    auto new_position = std::lower_bound(second_range, m_ranges.end(), m_ranges.front(), detail::compare_by_front_value(m_compare));
                    std::rotate(m_ranges.begin(), second_range, new_position);
                }
                else
                {
                    scroll_to_end();
                    break;
                }
            }
        }

        void scroll_to_end ()
        {
            m_ranges.clear();
        }

    public:
        typename base_type::reference dereference () const
        {
            return m_ranges.front().front();
        }

        bool equal (const intersect_iterator & that) const
        {
            return this->m_ranges == that.m_ranges;
        }

    private:
        std::vector<range_type> m_ranges;
        compare_type m_compare;
    };

    template <typename RangeRange, typename Compare>
    intersect_iterator
    <
        typename RangeRange::value_type,
        Compare
    >
    make_intersect_iterator (const RangeRange & ranges, Compare compare)
    {
        return intersect_iterator<typename RangeRange::value_type, Compare>(ranges, compare);
    }

    template <typename RangeRange>
    intersect_iterator
    <
        typename RangeRange::value_type
    >
    make_intersect_iterator (const RangeRange & ranges)
    {
        return intersect_iterator<typename RangeRange::value_type>(ranges);
    }

    template <typename RangeRange, typename Compare>
    intersect_iterator
    <
        typename RangeRange::value_type,
        Compare
    >
    make_intersect_iterator (const RangeRange &, Compare, iterator::end_tag_t)
    {
        return intersect_iterator<typename RangeRange::value_type, Compare>();
    }

    template <typename RangeRange>
    intersect_iterator
    <
        typename RangeRange::value_type
    >
    make_intersect_iterator (const RangeRange &, iterator::end_tag_t)
    {
        return intersect_iterator<typename RangeRange::value_type>();
    }
} // namespace thrust

#endif // THRUST_ITERATOR_INTERSECT_ITERATOR_HPP
