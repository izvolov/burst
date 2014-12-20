#ifndef THRUST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
#define THRUST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/numeric.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <thrust/iterator/end_tag.hpp>

namespace thrust
{
    namespace detail
    {
        template <typename ForwardRange, typename IteratorCategory>
        class join_iterator_base: public boost::iterator_facade
                                         <
                                             join_iterator_base<ForwardRange, IteratorCategory>,
                                             typename ForwardRange::value_type,
                                             boost::forward_traversal_tag,
                                             typename ForwardRange::reference
                                         >
        {
        private:
            typedef ForwardRange range_type;

            typedef boost::iterator_facade
            <
                join_iterator_base,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename range_type::reference
            >
            base_type;

        public:
            template <typename BidirectionalRange>
            explicit join_iterator_base (const BidirectionalRange & ranges):
                m_ranges()
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename BidirectionalRange::value_type, range_type>::value);
                boost::algorithm::copy_if
                (
                    boost::adaptors::reverse(ranges),
                    std::back_inserter(m_ranges),
                    not boost::bind(&range_type::empty, _1)
                );
            }

            template <typename BidirectionalRange>
            join_iterator_base (const BidirectionalRange &, iterator::end_tag_t):
                m_ranges()
            {
            }

            join_iterator_base () = default;

        private:
            friend class boost::iterator_core_access;

            void increment ()
            {
                m_ranges.back().advance_begin(1);
                if (m_ranges.back().empty())
                {
                    m_ranges.pop_back();
                }
            }

        private:
            typename base_type::reference dereference () const
            {
                return m_ranges.back().front();
            }

            bool equal (const join_iterator_base & that) const
            {
                return this->m_ranges == that.m_ranges;
            }

        private:
            std::vector<range_type> m_ranges;
        };

        template <typename RandomAccessRange>
        class join_iterator_base
        <
            RandomAccessRange,
            std::random_access_iterator_tag
        >
        : public boost::iterator_facade
                 <
                     join_iterator_base<RandomAccessRange, std::random_access_iterator_tag>,
                     typename RandomAccessRange::value_type,
                     boost::random_access_traversal_tag,
                     typename RandomAccessRange::reference
                 >
        {
        private:
            typedef RandomAccessRange range_type;
            typedef std::vector<range_type> range_container_type;

            typedef boost::iterator_facade
            <
                join_iterator_base,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename range_type::reference
            >
            base_type;

        public:
            template <typename InputRange>
            explicit join_iterator_base (const InputRange & ranges):
                m_ranges(std::make_shared<range_container_type>()),
                m_outer_range_index(0),
                m_inner_range_index(0),
                m_items_passed(0)
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename InputRange::value_type, range_type>::value);
                boost::algorithm::copy_if(ranges, std::back_inserter(*m_ranges), not boost::bind(&range_type::empty, _1));
            }

            template <typename InputRange>
            explicit join_iterator_base (const InputRange & ranges, iterator::end_tag_t):
                m_ranges(std::make_shared<range_container_type>()),
                m_outer_range_index(0),
                m_inner_range_index(0),
                m_items_passed(0)
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename InputRange::value_type, range_type>::value);
                boost::algorithm::copy_if(ranges, std::back_inserter(*m_ranges), not boost::bind(&range_type::empty, _1));
                m_items_passed = boost::accumulate
                (
                    *m_ranges,
                    0u,
                    boost::bind(std::plus<typename range_type::size_type>(), _1, boost::bind(&range_type::size, _2))
                );
                m_outer_range_index = m_ranges->size();
            }

            join_iterator_base () = default;

        private:
            friend class boost::iterator_core_access;

            void advance (typename base_type::difference_type n)
            {
                m_items_passed += static_cast<typename range_type::size_type>(n);

                if (n > 0)
                {
                    forward(n);
                }
                else
                {
                    backward(n);
                }
            }

            void forward (typename base_type::difference_type n)
            {
                while (n > 0)
                {
                    auto items_remaining_in_current_range = (*m_ranges)[m_outer_range_index].size() - m_inner_range_index;
                    auto items_to_scroll_in_current_range =
                        std::min
                        (
                            static_cast<typename range_type::size_type>(n),
                            items_remaining_in_current_range
                        );
                    n -= items_to_scroll_in_current_range;

                    m_inner_range_index += items_to_scroll_in_current_range;
                    if (m_inner_range_index == (*m_ranges)[m_outer_range_index].size())
                    {
                        ++m_outer_range_index;
                        m_inner_range_index = 0;
                    }
                }
            }

            void backward (typename base_type::difference_type n)
            {
                while (n < 0)
                {
                    auto items_remaining_in_current_range = m_inner_range_index;
                    auto items_to_scroll_in_current_range =
                        std::min
                        (
                            static_cast<typename range_type::size_type>(std::abs(n)),
                            items_remaining_in_current_range
                        );
                    n += items_to_scroll_in_current_range;

                    m_inner_range_index -= items_to_scroll_in_current_range;
                    if (m_inner_range_index == 0)
                    {
                        --m_outer_range_index;
                        m_inner_range_index = (*m_ranges)[m_outer_range_index].size();
                    }
                }
            }

            void increment ()
            {
                ++m_inner_range_index;
                if (m_inner_range_index == (*m_ranges)[m_outer_range_index].size())
                {
                    ++m_outer_range_index;
                    m_inner_range_index = 0;
                }

                ++m_items_passed;
            }

            void decrement ()
            {
                if (m_inner_range_index == 0)
                {
                    --m_outer_range_index;
                    m_inner_range_index = (*m_ranges)[m_outer_range_index].size() - 1;
                }

                --m_items_passed;
            }

        private:
            typename base_type::reference dereference () const
            {
                auto inner_range_index = static_cast<typename base_type::difference_type>(m_inner_range_index);
                return (*m_ranges)[m_outer_range_index][inner_range_index];
            }

            bool equal (const join_iterator_base & that) const
            {
                return this->m_items_passed == that.m_items_passed;
            }

            typename base_type::difference_type distance_to (const join_iterator_base & that) const
            {
                return static_cast<typename base_type::difference_type>(that.m_items_passed - this->m_items_passed);
            }

        private:
            std::shared_ptr<range_container_type> m_ranges;

            typename std::vector<range_type>::size_type m_outer_range_index;
            typename range_type::size_type m_inner_range_index;

            typename range_type::size_type m_items_passed;
        };
    }
}

#endif // THRUST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
