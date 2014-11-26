#ifndef THRUST_ITERATOR_MERGE_ITERATOR_HPP
#define THRUST_ITERATOR_MERGE_ITERATOR_HPP

#include <algorithm>
#include <vector>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <thrust/iterator/detail/front_value_compare.hpp>
#include <thrust/iterator/detail/invert_compare.hpp>

namespace thrust
{
    template
    <
        typename InputRange,
        typename Compare = std::less<typename InputRange::value_type>
    >
    class merge_iterator: public boost::iterator_facade
                                 <
                                     merge_iterator<InputRange, Compare>,
                                     typename InputRange::value_type,
                                     boost::forward_traversal_tag,
                                     typename InputRange::reference
                                 >
    {
    private:
        typedef InputRange range_type;

        typedef boost::iterator_facade
        <
            merge_iterator,
            typename range_type::value_type,
            boost::forward_traversal_tag,
            typename range_type::reference
        >
        base_type;

    public:
        template <typename InputRange1>
        explicit merge_iterator (const InputRange1 & ranges, Compare compare = Compare()):
            m_range_heap(),
            m_heap_order(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename InputRange1::value_type, range_type>::value);
            BOOST_ASSERT(boost::algorithm::all_of(m_range_heap.begin(), m_range_heap.end(), boost::bind(&boost::is_sorted<range_type, Compare>, _1, compare)));

            m_range_heap.reserve(ranges.size());
            boost::algorithm::copy_if(ranges, std::back_inserter(m_range_heap), not boost::bind(&range_type::empty, _1));
            std::make_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
        }

        merge_iterator ():
            m_range_heap(),
            m_heap_order(Compare())
        {
        }

    public:
        void increment ()
        {
            std::pop_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            range_type & range = m_range_heap.back();

            range.advance_begin(1);
            if (not range.empty())
            {
                std::push_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            }
            else
            {
                m_range_heap.pop_back();
            }
        }

    public:
        typename base_type::reference dereference () const
        {
            return m_range_heap.front().front();
        }

        bool equal (const merge_iterator & that) const
        {
            return this->m_range_heap == that.m_range_heap;
        }

    private:
        std::vector<range_type> m_range_heap;

        // invert_comparison устраняет путаницу с обратным порядком в пирамиде при работе с std::make(push, pop)_heap.
        typedef detail::front_value_comparator<detail::invert_comparison<Compare>> heap_order_type;
        heap_order_type m_heap_order;
    };
} // namespace thrust

#endif // THRUST_ITERATOR_MERGE_ITERATOR_HPP
