#ifndef BURST_CONTAINER_K_ARY_SEARCH_SET_HPP
#define BURST_CONTAINER_K_ARY_SEARCH_SET_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <stack>
#include <utility>
#include <vector>

#include <boost/assert.hpp>
#include <boost/container/container_fwd.hpp>
#include <boost/range/iterator_range.hpp>

#include <burst/container/unique_ordered_tag.hpp>

namespace burst
{
    struct k_ary_search_set_branch
    {
        std::size_t index;
        std::size_t size;
        std::size_t height;
        std::size_t preceding_elements;
    };

    template<typename Value, typename Compare = std::less<Value>>
    class k_ary_search_set
    {
    public:
        using value_type = Value;
        using value_compare = Compare;

    private:
        using value_container_type = std::vector<value_type>;

    public:
        using iterator = typename value_container_type::iterator;
        using const_iterator = typename value_container_type::const_iterator;
        using size_type = typename value_container_type::size_type;
        using difference_type = typename value_container_type::difference_type;

    public:
        template <typename RandomAccessIterator>
        k_ary_search_set
                (
                    container::unique_ordered_tag_t,
                    RandomAccessIterator first,
                    RandomAccessIterator last,
                    std::size_t arity = default_arity,
                    const value_compare & compare = value_compare()
                ):
            m_values(static_cast<size_type>(std::distance(first, last))),
            m_arity(arity),
            m_compare(compare)
        {
            BOOST_ASSERT(std::adjacent_find(first, last, std::not2(compare)) == last);
            initialize_trusted(boost::make_iterator_range(first, last));
        }

        template <typename RandomAccessIterator>
        k_ary_search_set
                (
                    RandomAccessIterator first,
                    RandomAccessIterator last,
                    std::size_t arity = default_arity,
                    const value_compare & compare = value_compare()
                ):
            m_arity(arity),
            m_compare(compare)
        {
            initialize_preparing(boost::make_iterator_range(first, last));
        }

        k_ary_search_set
                (
                    container::unique_ordered_tag_t,
                    std::initializer_list<value_type> values,
                    std::size_t arity = default_arity,
                    const value_compare & compare = value_compare()
                ):
            m_values(values.size()),
            m_arity(arity),
            m_compare(compare)
        {
            BOOST_ASSERT(std::adjacent_find(values.begin(), values.end(), std::not2(compare)) == values.end());
            initialize_trusted(boost::make_iterator_range(values));
        }

        k_ary_search_set
                (
                    std::initializer_list<value_type> values,
                    std::size_t arity = default_arity,
                    const value_compare & compare = value_compare()
                ):
            m_arity(arity),
            m_compare(compare)
        {
            initialize_preparing(values);
        }

        k_ary_search_set ():
            m_arity(0)
        {
        }

    public:
        iterator find (const value_type & value)
        {
            return begin() + std::distance(cbegin(), find_impl(value));
        }

        const_iterator find (const value_type & value) const
        {
            return find_impl(value);
        }

        size_type size () const
        {
            return m_values.size();
        }

        bool empty () const
        {
            return m_values.empty();
        }

        iterator begin ()
        {
            return m_values.begin();
        }

        iterator end ()
        {
            return m_values.end();
        }

        const_iterator begin () const
        {
            return m_values.begin();
        }

        const_iterator end () const
        {
            return m_values.end();
        }

        const_iterator cbegin () const
        {
            return m_values.cbegin();
        }

        const_iterator cend () const
        {
            return m_values.cend();
        }

    private:
        const_iterator find_impl (const value_type & value) const
        {
            std::size_t node_index = 0;

            while (node_index < m_values.size())
            {
                const_iterator node_begin = begin() + static_cast<difference_type>(node_index);
                const_iterator node_end = node_begin + std::min(static_cast<difference_type>(m_arity - 1), std::distance(node_begin, end()));

                const_iterator search_result = std::lower_bound(node_begin, node_end, value, m_compare);
                if (search_result != node_end && not m_compare(value, *search_result))
                {
                    return search_result;
                }
                else
                {
                    node_index = perfect_tree_child_index
                    (
                        m_arity,
                        node_index,
                        static_cast<std::size_t>(std::distance(node_begin, search_result))
                    );
                }
            }

            return end();
        }

        template <typename RandomAccessRange>
        void initialize_preparing (const RandomAccessRange & range)
        {
            value_container_type buffer(range.begin(), range.end());
            std::sort(buffer.begin(), buffer.end(), m_compare);
            buffer.erase(std::unique(buffer.begin(), buffer.end(), std::not2(m_compare)), buffer.end());

            initialize_trusted(boost::make_iterator_range(buffer));
        }

        template <typename RandomAccessRange>
        void initialize_trusted (const RandomAccessRange & range)
        {
            if (not range.empty())
            {
                m_values.resize(range.size());

                std::stack<k_ary_search_set_branch> branches;

                branches.push({0, size(), perfect_tree_height(m_arity, size()), 0});
                while (not branches.empty())
                {
                    const auto branch = branches.top();
                    branches.pop();

                    // Количество меньших элементов ветки для каждого элемента текущего узла.
                    std::vector<std::size_t> counters;
                    fill_counters(branch, counters);

                    fill_node(branch, counters, range);

                    if (counters[0] > 0)
                    {
                        branches.push
                        ({
                            perfect_tree_child_index(m_arity, branch.index, 0),
                            counters[0],
                            branch.height - 1,
                            branch.preceding_elements
                        });
                    }

                    for (std::size_t i = 1; i < counters.size() && (counters[i] - counters[i - 1] - 1) > 0; ++i)
                    {
                        branches.push
                        ({
                            perfect_tree_child_index(m_arity, branch.index, i),
                            counters[i] - counters[i - 1] - 1,
                            branch.height - 1,
                            branch.preceding_elements + counters[i - 1] + 1
                        });
                    }
                }
            }
        }

        void fill_counters (const k_ary_search_set_branch & branch, std::vector<std::size_t> & counters)
        {
            const std::size_t max_subtree_height = branch.height - 1;
            const std::size_t min_subtree_elements = perfect_tree_size(m_arity, max_subtree_height - 1);
            const std::size_t max_subtree_elements = perfect_tree_size(m_arity, max_subtree_height);
            const std::size_t elements_in_last_row = branch.size - perfect_tree_size(m_arity, branch.height - 1);

            counters.resize(std::min(m_arity, branch.size + 1));
            for (std::size_t i = 0; i < counters.size(); ++i)
            {
                counters[i] = i + std::min
                (
                    (i + 1) * min_subtree_elements + elements_in_last_row,
                    (i + 1) * max_subtree_elements
                );
            }
            BOOST_ASSERT(counters.back() == branch.size);
        }

        template <typename RandomAccessRange>
        void fill_node (const k_ary_search_set_branch & branch, const std::vector<std::size_t> & counters, const RandomAccessRange & range)
        {
            for (std::size_t element_index = 0; element_index < counters.size() - 1; ++element_index)
            {
                using difference_type = typename RandomAccessRange::difference_type;
                const auto index_in_initial_range = static_cast<difference_type>(branch.preceding_elements + counters[element_index]);
                m_values[branch.index + element_index] = range[index_in_initial_range];
            }
            BOOST_ASSERT(std::is_sorted
            (
                m_values.begin() + static_cast<typename value_container_type::difference_type>(branch.index),
                m_values.begin() + static_cast<typename value_container_type::difference_type>(branch.index + counters.size() - 1),
                m_compare
            ));
        }

    private:
        static std::size_t perfect_tree_size (std::size_t arity, std::size_t height)
        {
            return static_cast<std::size_t>(std::pow(arity, height)) - 1;
        }

        static std::size_t perfect_tree_height (std::size_t arity, std::size_t size)
        {
            return logip(size, arity) + 1;
        }

        static std::size_t perfect_tree_child_index (std::size_t arity, std::size_t parent_index, std::size_t child_number)
        {
            return parent_index * arity + (child_number + 1) * (arity - 1);
        }

        //!     Целая часть логарифма.
        /*!
                Вычисляет целую часть логарифма числа по заданному основанию.
                logip = LOGarithm Integral Part.
         */
        static std::size_t logip (std::size_t number, std::size_t base)
        {
            std::size_t degree = 0;
            std::size_t exponent = 1;

            while (exponent < number)
            {
                exponent *= base;
                ++degree;
            }

            if (exponent == number)
            {
                return degree;
            }
            else
            {
                return degree - 1;
            }
        }

    private:
        static const std::size_t default_arity = 33;

    private:
        value_container_type m_values;
        const std::size_t m_arity;
        value_compare m_compare;
    };
}

#endif // BURST_CONTAINER_K_ARY_SEARCH_SET_HPP
