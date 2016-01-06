#ifndef BURST_ITERATOR_SUBSET_ITERATOR_HPP
#define BURST_ITERATOR_SUBSET_ITERATOR_HPP

#include <burst/iterator/detail/subset.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/detail/has_advance_begin.hpp>

#include <boost/assert.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace burst
{
    //!     Итератор подмножеств.
    /*!
            Предназначен для того, чтобы перебирать подмножества диапазонов без лишних копирований
        элементов этих диапазонов.
            Принимает на вход упорядоченный диапазон элементов, а на выход отдаёт диапазоны, каждый
        из которых представляет некоторое подмножество исходного диапазона.
            Подмножество представлено диапазоном, пробежав по которому можно получить элементы
        исходного диапазона, входящие в это подмножество.

        \tparam ForwardRange
            Тип диапазона, подмножества которого нужно перебрать. Он должен быть хотя бы
            однонаправленным, то есть удовлетворять требованиям понятия "Forward Range".
        \tparam Compare
            Двухместная операция, задающая отношение строгого порядка на элементах входного
            диапазона.
                bool Compare (Value [const &], Value [const &])
            Если не указана явно, то по-умолчанию берётся "std::less<T>".
        \tparam SubsetContainer
            Тип контейнера, с помощью которого задаются подмножества.
            Хранит итераторы на элементы исходного диапазона.

            Принцип работы.

        0. Пустое множество не входит в набор подмножеств.
        1. Итератор на пустое подмножество — это конец (end-итератор) списка подмножеств.
        2. Подмножества перебираются в порядке увеличения их размера, а в рамках одного размера — в
           лексикографически возрастающем порядке относительно заданного отношения порядка их
           элементов.
           Последним действительным подмножеством будет множество, состоящее из всех уникальных
           элементов исходного диапазона.
        3. После максимального подмножества итератор возвращается к пустому подмножеству.
        4. Одинаковых элементов в подмножестве не бывает.
     */
    template
    <
        typename ForwardRange,
        typename Compare = std::less<>,
        typename SubsetContainer = std::vector<typename ForwardRange::const_iterator>
    >
    class subset_iterator:
        public boost::iterator_facade
        <
            subset_iterator<ForwardRange, Compare, SubsetContainer>,
            boost::iterator_range<boost::indirect_iterator<typename SubsetContainer::const_iterator>>,
            boost::forward_traversal_tag,
            boost::iterator_range<boost::indirect_iterator<typename SubsetContainer::const_iterator>>
        >
    {
    private:
        static_assert(detail::has_advance_begin<ForwardRange>::value, "Контейнеры не допускаются, только диапазоны");

        using range_type = ForwardRange;
        using compare_type = Compare;
        using subset_container_type = SubsetContainer;

        using base_type = boost::iterator_facade
        <
            subset_iterator,
            boost::iterator_range<boost::indirect_iterator<typename subset_container_type::const_iterator>>,
            boost::forward_traversal_tag,
            boost::iterator_range<boost::indirect_iterator<typename subset_container_type::const_iterator>>
        >;

    public:
        explicit subset_iterator (range_type range, compare_type compare = compare_type{}):
            m_range(std::move(range)),
            m_subset(),
            m_compare(compare)
        {
            BOOST_ASSERT(std::is_sorted(m_range.begin(), m_range.end(), compare));
            detail::next_subset(m_range, m_subset, m_compare);
        }

        subset_iterator (const subset_iterator & begin, iterator::end_tag_t):
            m_range(begin.m_range),
            m_subset(),
            m_compare(begin.m_compare)
        {
        }

        subset_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            detail::next_subset(m_range, m_subset, m_compare);
        }

        typename base_type::reference dereference () const
        {
            return boost::make_iterator_range
            (
                boost::make_indirect_iterator(m_subset.begin()),
                boost::make_indirect_iterator(m_subset.end())
            );
        }

        bool equal (const subset_iterator & that) const
        {
            BOOST_ASSERT(this->m_range == that.m_range);
            return this->m_subset == that.m_subset;
        }

    private:
        range_type m_range;
        subset_container_type m_subset;
        compare_type m_compare;
    };

    //!     Функция для создания итератора подмножеств с предикатом.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона. Сам диапазон должен быть
        упорядочен относительно этой операции.
            Возвращает либо итератор на минимальное непустое подмножество входного диапазона, либо,
        если ни одного непустого подмножества не найдено, итератор на пустое множество.
     */
    template <typename ForwardRange, typename Compare>
    auto make_subset_iterator (ForwardRange range, Compare compare)
    {
        return subset_iterator<ForwardRange, Compare>(std::move(range), compare);
    }

    //!     Функция для создания итератора подмножеств.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать.
            Возвращает либо итератор на минимальное непустое подмножество входного диапазона, либо,
        если ни одного непустого подмножества не найдено, итератор на пустое множество.
            Отношение порядка на элементах диапазона выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    auto make_subset_iterator (ForwardRange range)
    {
        return subset_iterator<ForwardRange>(std::move(range));
    }

    //!     Функция для создания итератора на конец подмножеств.
    /*!
            Принимает на вход итератор на начало подмножеств и индикатор создания итератора на
        конец подмножеств.
            Возвращает итератор на пустое подмножество входного диапазона.
            Отношение порядка на элементах диапазона выбирается по-умолчанию.
     */
    template <typename ForwardRange, typename Compare>
    auto make_subset_iterator (const subset_iterator<ForwardRange, Compare> & begin, iterator::end_tag_t)
    {
        return subset_iterator<ForwardRange, Compare>(begin, iterator::end_tag);
    }
} // namespace burst

#endif // BURST_ITERATOR_SUBSET_ITERATOR_HPP
