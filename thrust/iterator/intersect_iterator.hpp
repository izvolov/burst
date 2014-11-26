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
                                     typename InputRange::reference
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
            typename range_type::reference
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
                scroll_range_to(m_ranges.front(), m_ranges.back().front());
                if (not m_ranges.front().empty())
                {
                    put_front_range_to_its_place();
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

        void scroll_range_to (range_type & scrolling_range, const typename base_type::value_type & goal)
        {
            auto position_to_scroll = std::lower_bound(scrolling_range.begin(), scrolling_range.end(), goal, m_compare);
            scrolling_range = boost::make_iterator_range(position_to_scroll, scrolling_range.end());
        }

        void put_front_range_to_its_place ()
        {
            auto second_range = ++m_ranges.begin();

            auto position_in_sorted_range = std::lower_bound
            (
                second_range,
                m_ranges.end(),
                m_ranges.front(),
                detail::compare_by_front_value(m_compare)
            );
            std::rotate(m_ranges.begin(), second_range, position_in_sorted_range);
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
    boost::iterator_range
    <
        intersect_iterator<typename RangeRange::value_type, Compare>
    >
    intersect (const RangeRange & ranges, Compare compare)
    {
        typedef intersect_iterator<typename RangeRange::value_type, Compare> iterator;
        return boost::make_iterator_range(iterator(ranges, compare), iterator());
    }

    template <typename RangeRange>
    boost::iterator_range
    <
        intersect_iterator<typename RangeRange::value_type>
    >
    intersect (const RangeRange & ranges)
    {
        typedef intersect_iterator<typename RangeRange::value_type> iterator;
        return boost::make_iterator_range(iterator(ranges), iterator());
    }
} // namespace thrust

#endif // THRUST_ITERATOR_INTERSECT_ITERATOR_HPP
