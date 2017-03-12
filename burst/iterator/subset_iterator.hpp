#ifndef BURST_ITERATOR_SUBSET_ITERATOR_HPP
#define BURST_ITERATOR_SUBSET_ITERATOR_HPP

#include <burst/algorithm/next_subset.hpp>
#include <burst/iterator/end_tag.hpp>

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

        \tparam ForwardIterator
            Тип итератора диапазона, подмножества которого нужно перебрать. Он должен быть хотя бы
            однонаправленным, то есть удовлетворять требованиям понятия "Forward Iterator".
        \tparam Compare
            Двухместная операция, задающая отношение строгого порядка на элементах входного
            диапазона.
                bool Compare (Value [const &], Value [const &])
            Если не указана явно, то по-умолчанию берётся "std::less<>".
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
        typename ForwardIterator,
        typename Compare = std::less<>,
        typename SubsetContainer = std::vector<ForwardIterator>
    >
    class subset_iterator:
        public boost::iterator_facade
        <
            subset_iterator<ForwardIterator, Compare, SubsetContainer>,
            boost::iterator_range<boost::indirect_iterator<typename SubsetContainer::const_iterator>>,
            boost::single_pass_traversal_tag,
            boost::iterator_range<boost::indirect_iterator<typename SubsetContainer::const_iterator>>
        >
    {
    private:
        using compare_type = Compare;
        using subset_container_type = SubsetContainer;

        using base_type = boost::iterator_facade
        <
            subset_iterator,
            boost::iterator_range<boost::indirect_iterator<typename subset_container_type::const_iterator>>,
            boost::single_pass_traversal_tag,
            boost::iterator_range<boost::indirect_iterator<typename subset_container_type::const_iterator>>
        >;

    public:
        explicit subset_iterator
            (
                ForwardIterator first, ForwardIterator last,
                compare_type compare = compare_type{}
            ):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_subset(static_cast<std::size_t>(std::distance(m_begin, m_end))),
            m_subset_size(0),
            m_compare(compare)
        {
            BOOST_ASSERT(std::is_sorted(m_begin, m_end, m_compare));
            increment();
        }

        subset_iterator (iterator::end_tag_t, const subset_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_end),
            m_subset{},
            m_subset_size{},
            m_compare(begin.m_compare)
        {
        }

        subset_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            const auto new_subset_end =
                next_subset(m_begin, m_end, subset_begin(), subset_end(), m_compare);
            m_subset_size = std::distance(subset_begin(), new_subset_end);
        }

        typename base_type::reference dereference () const
        {
            return boost::make_iterator_range
            (
                boost::make_indirect_iterator(subset_begin()),
                boost::make_indirect_iterator(subset_end())
            );
        }

        bool equal (const subset_iterator & that) const
        {
            BOOST_ASSERT(this->m_begin == that.m_begin);
            BOOST_ASSERT(this->m_end == that.m_end);
            return
                std::equal
                (
                    this->subset_begin(), this->subset_end(),
                    that.subset_begin(), that.subset_end()
                );
        }

        auto subset_begin () const
        {
            return m_subset.begin();
        }

        auto subset_end () const
        {
            return subset_begin() + m_subset_size;
        }

        auto subset_end ()
        {
            return subset_begin() + m_subset_size;
        }

        auto subset_begin ()
        {
            return m_subset.begin();
        }

    private:
        ForwardIterator m_begin;
        ForwardIterator m_end;
        subset_container_type m_subset;
        typename subset_container_type::difference_type m_subset_size;
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
    template <typename ForwardIterator, typename Compare>
    auto make_subset_iterator (ForwardIterator first, ForwardIterator last, Compare compare)
    {
        return
            subset_iterator<ForwardIterator, Compare>(std::move(first), std::move(last), compare);
    }

    template <typename ForwardRange, typename Compare>
    auto make_subset_iterator (ForwardRange && range, Compare compare)
    {
        return
            make_subset_iterator
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range)),
                compare
            );
    }

    //!     Функция для создания итератора подмножеств.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать.
            Возвращает либо итератор на минимальное непустое подмножество входного диапазона, либо,
        если ни одного непустого подмножества не найдено, итератор на пустое множество.
            Отношение порядка на элементах диапазона выбирается по-умолчанию.
     */
    template <typename ForwardIterator>
    auto make_subset_iterator (ForwardIterator first, ForwardIterator last)
    {
        return subset_iterator<ForwardIterator>(std::move(first), std::move(last));
    }

    template <typename ForwardRange>
    auto make_subset_iterator (ForwardRange && range)
    {
        return
            make_subset_iterator
            (
                std::begin(std::forward<ForwardRange>(range)),
                std::end(std::forward<ForwardRange>(range))
            );
    }

    //!     Функция для создания итератора на конец подмножеств.
    /*!
            Принимает на вход итератор на начало подмножеств и индикатор создания итератора на
        конец подмножеств.
            Возвращает итератор на пустое подмножество входного диапазона.
            Отношение порядка на элементах диапазона выбирается по-умолчанию.
     */
    template <typename ForwardIterator, typename Compare>
    auto make_subset_iterator (iterator::end_tag_t, const subset_iterator<ForwardIterator, Compare> & begin)
    {
        return subset_iterator<ForwardIterator, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_SUBSET_ITERATOR_HPP
