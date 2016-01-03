#ifndef BURST_CONTAINER_K_ARY_SEARCH_SET_HPP
#define BURST_CONTAINER_K_ARY_SEARCH_SET_HPP

#include <burst/container/unique_ordered_tag.hpp>
#include <burst/math/intlog.hpp>
#include <burst/math/intpow.hpp>

#include <boost/assert.hpp>
#include <boost/container/container_fwd.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <stack>
#include <utility>
#include <vector>

namespace burst
{
    struct k_ary_search_set_branch
    {
        std::size_t index;
        std::size_t size;
        std::size_t height;
        std::size_t preceding_elements;
    };

    //!     Множество, основанное на k-местном дереве поиска.
    /*!
            k-местное дерево поиска — это дерево поиска, у которого в каждом узле находится не
        более (k - 1) элементов и не более k переходов в поддеревья.
            В данном случае используется правильное k-местное дерево, то есть такое, в котором
        обязательно заполнены все уровни дерева, кроме последнего, причём в последнем уровне
        элементы прибиты к левому краю уровня.
            При этом, естественно, соблюдается, условие дерева поиска, то есть все элементы,
        находящиеся в i-м поддереве узла, строго меньше i-го элемента узла и не меньше (i - 1)-го
        элемента узла, а i-й элемент узла не больше (i + 1)-го элемента узла.
            Повторяющихся элементов в дереве нет, это не мультимножество.
            Само дерево представлено в виде массива, элементы в котором размещены по принципу,
        схожему с пирамидой.

        \tparam Value
            Тип данных, хранящихся в множестве.
        \tparam Compare
            Отношение порядка, по которому элементы выстроены в дереве.
            Должно быть двухместной операцией на элементах типа Value, возвращающей булевское
            значение.

                bool Compare (Value [const &], Value [const &])
     */
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
        //!     Создание множества из упорядоченного набора, представленного итераторами.
        /*!
                Принимает два итератора, которые задают набор элементов, которые должны быть в
            результирующем множестве, местность дерева, отношение порядка на элементах, а также
            фиктивный элемент — метку, обозначающую, что входной набор элементов уже упорядочен и
            элементы в нём уникальны, то есть проводить дополнительную предобработку не нужно.

                Асимптотика.

            Время: O(N), N = |[first, last)| — размер дерева.
            Память: O(log_k(N) + k).
                Учитывается только собственная память конструктора. Созданное дерево, естественно,
                содержит ровно N элементов.
         */
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
            initialize_trusted(boost::make_iterator_range(first, last));
        }

        //!     Создание множества из набора, заданного итераторами.
        /*!
                Принимает два итератора, которые задают набор элементов, которые должны быть в
            результирующем множестве, местность дерева и отношение порядка на элементах.

                Асимптотика.

            Время:
                1. O(N), если набор упорядочен.
                2. O(N logN), если набор неупорядочен,
                где N = |[first, last)|.
            Память: O(log_k(N) + k).
         */
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
            initialize(boost::make_iterator_range(first, last));
        }

        //!     Создание множества из упорядоченного набора, представленного списком инициализации.
        /*!
                Принимает std::initializer_list, заполненный элементами которые должны быть в
            результирующем множестве, местность дерева, отношение порядка на элементах, а также
            фиктивный элемент — метку, обозначающую, что входной набор элементов уже упорядочен и
            элементы в нём уникальны, то есть проводить дополнительную предобработку не нужно.

                Асимптотика.

            Время: O(N), где N = |values|.
            Память: O(log_k(N) + k).
         */
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
            initialize_trusted(boost::make_iterator_range(values));
        }

        //!     Создание множества из набора, представленного списком инициализации.
        /*!
                Принимает std::initializer_list, заполненный элементами которые должны быть в
            результирующем множестве, местность дерева и отношение порядка на элементах.

                Асимптотика.

            Время:
                1. O(N), если набор упорядочен.
                2. O(N logN), если набор неупорядочен,
                где N = |values|.
            Память: O(log_k(N) + k).
         */
        k_ary_search_set
                (
                    std::initializer_list<value_type> values,
                    std::size_t arity = default_arity,
                    const value_compare & compare = value_compare()
                ):
            m_arity(arity),
            m_compare(compare)
        {
            initialize(boost::make_iterator_range(values));
        }

        k_ary_search_set ():
            m_arity(0)
        {
        }

    public:
        //!     Поиск элемента в множестве.
        /*!
                Если искомый элемент существует в множестве, то возвращается итератор на него. Если
            не существует, то возвращается end().

                Асимптотика.

            Время: O(log_k(N)),
                k — местность дерева.
                N — количество элементов в дереве.
            Память: O(1).
         */
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

        //!     Начало множества.
        /*!
                Важно, что последовательность [begin(), end()) неупорядочена.
         */
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
        void initialize (const RandomAccessRange & range)
        {
            if (std::is_sorted(range.begin(), range.end(), m_compare))
            {
                initialize_trusted(range);
            }
            else
            {
                value_container_type buffer(range.begin(), range.end());
                std::sort(buffer.begin(), buffer.end(), m_compare);
                buffer.erase(std::unique(buffer.begin(), buffer.end(), std::not2(m_compare)), buffer.end());

                initialize_trusted(boost::make_iterator_range(buffer));
            }
        }

        //!     Расстановка элементов по своим местам.
        /*!
                Обходит дерево по уровням и расставляет по местам в дереве элементы исходной
            последовательности.

                Асимптотика.

            Время: O(N), N — количество элементов в дереве.
            Память: O(log_k(N) + k),
                O(log_k(N)) памяти требуется для хранения стека при обходе узлов дерева.
                O(k) памяти требуется для хранения счётчиков при заполнении одного узла.
         */
        template <typename RandomAccessRange>
        void initialize_trusted (const RandomAccessRange & range)
        {
            BOOST_ASSERT(std::adjacent_find(range.begin(), range.end(), std::not2(m_compare)) == range.end());
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

        //!     Подсчёт счётчиков для элементов узла.
        /*!
                Для каждого элемента узла подсчитывает количество элементов в ветке (ветка включает
            рассматриваемый узел), которые строго меньше этого элемента.
         */
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

        //!     Заполнение узла нужными элементами исходного диапазона.
        /*!
                Для каждого элемента узла известен индекс в исходном диапазоне, по которому лежит
            нужное значение. Осталось только скопировать его.
         */
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
            return intpow(arity, height) - 1;
        }

        static std::size_t perfect_tree_height (std::size_t arity, std::size_t size)
        {
            return intlog(size, arity) + 1;
        }

        static std::size_t perfect_tree_child_index (std::size_t arity, std::size_t parent_index, std::size_t child_number)
        {
            return parent_index * arity + (child_number + 1) * (arity - 1);
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
