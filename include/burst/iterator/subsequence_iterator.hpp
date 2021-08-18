#ifndef BURST__ITERATOR__SUBSEQUENCE_ITERATOR_HPP
#define BURST__ITERATOR__SUBSEQUENCE_ITERATOR_HPP

#include <burst/algorithm/next_subsequence.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

namespace burst
{
    /*!
        \~russian
            \brief
                Итератор подпоследовательностей

            \details
                Предназначен для того, чтобы перебирать подпоследовательности диапазонов без лишних
                копирований элементов этих диапазонов.
                Принимает на вход диапазон элементов, а на выход отдаёт диапазоны, каждый из
                которых представляет некоторую подпоследовательность исходного диапазона.
                Подпоследовательность представлена диапазоном, пробежав по которому можно получить
                элементы исходного диапазона, входящие в эту подпоследовательность.

                Принцип работы:
                0.  Пустая подпоследовательность не входит в набор подпоследовательностей;
                1.  Итератор на пустую подпоследовательность считается концом (end-итератором)
                    списка подпоследовательностей;
                2.  Подспоследовательности перебираются в порядке увеличения их размера, а в рамках
                    одного размера — в лексикографически возрастающем порядке относительно номеров
                    их элементов. Последней подпоследовательностью будет собственная
                    подпоследовательность, то есть такая, которая состоит из всех элементов
                    исходного диапазона;
                3.  После максимальной подпоследовательности итератор возвращается к пустой
                    подпоследовательности.

            \tparam ForwardIterator
                Тип итератора последовательности, подпоследовательности которой нужно перебрать.
                Он должен быть хотя бы однонаправленным, то есть удовлетворять требованиям понятия
                `Forward Iterator`.
            \tparam SubsequenceContainer
                Тип контейнера, с помощью которого хранятся подпоследовательности.
                Хранит итераторы на элементы исходного диапазона.
     */
    template
    <
        typename ForwardIterator,
        typename SubsequenceContainer = std::vector<ForwardIterator>
    >
    class subsequence_iterator:
        public boost::iterator_facade
        <
            subsequence_iterator<ForwardIterator, SubsequenceContainer>,
            boost::iterator_range
            <
                boost::indirect_iterator<typename SubsequenceContainer::const_iterator>
            >,
            boost::single_pass_traversal_tag,
            boost::iterator_range
            <
                boost::indirect_iterator<typename SubsequenceContainer::const_iterator>
            >
        >
    {
    private:
        using subsequence_container_type = SubsequenceContainer;

        using base_type =
            boost::iterator_facade
            <
                subsequence_iterator,
                boost::iterator_range
                <
                    boost::indirect_iterator<typename subsequence_container_type::const_iterator>
                >,
                boost::single_pass_traversal_tag,
                boost::iterator_range
                <
                    boost::indirect_iterator<typename subsequence_container_type::const_iterator>
                >
            >;

    public:
        subsequence_iterator (ForwardIterator first, ForwardIterator last):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_subsequence(static_cast<std::size_t>(std::distance(m_begin, m_end))),
            m_subsequence_size(0)
        {
            increment();
        }

        subsequence_iterator (iterator::end_tag_t, const subsequence_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_end),
            m_subsequence{},
            m_subsequence_size{}
        {
        }

        subsequence_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            const auto new_subsequence_end =
                next_subsequence(subsequence_begin(), subsequence_end(), m_begin, m_end);
            m_subsequence_size = std::distance(subsequence_begin(), new_subsequence_end);
        }

        typename base_type::reference dereference () const
        {
            return boost::make_iterator_range
            (
                boost::make_indirect_iterator(subsequence_begin()),
                boost::make_indirect_iterator(subsequence_end())
            );
        }

        bool equal (const subsequence_iterator & that) const
        {
            BOOST_ASSERT(this->m_begin == that.m_begin);
            BOOST_ASSERT(this->m_end == that.m_end);
            return
                std::equal
                (
                    this->subsequence_begin(), this->subsequence_end(),
                    that.subsequence_begin(), that.subsequence_end()
                );
        }

        auto subsequence_begin () const
        {
            return m_subsequence.begin();
        }

        auto subsequence_end () const
        {
            return subsequence_begin() + m_subsequence_size;
        }

        auto subsequence_end ()
        {
            return subsequence_begin() + m_subsequence_size;
        }

        auto subsequence_begin ()
        {
            return m_subsequence.begin();
        }

    private:
        ForwardIterator m_begin;
        ForwardIterator m_end;
        subsequence_container_type m_subsequence;
        typename subsequence_container_type::difference_type m_subsequence_size;
    };

    /*!
        \~russian
            \brief
                Функция для создания итератора подпоследовательностей

            \param [first, last)
                Полуинтервал, задающий исходную последовательность, подпоследовательности которой
                нужно перебрать.

            \returns
                Либо итератор на минимальную непустую подпоследовательность входного диапазона,
                либо, если ни одной непустой подпоследовательности не найдено, итератор на пустую
                подпоследовательность.
     */
    template <typename ForwardIterator>
    auto make_subsequence_iterator (ForwardIterator first, ForwardIterator last)
    {
        return subsequence_iterator<ForwardIterator>(std::move(first), std::move(last));
    }

    template <typename ForwardRange>
    auto make_subsequence_iterator (ForwardRange && range)
    {
        using std::begin;
        using std::end;
        return
            make_subsequence_iterator
            (
                begin(std::forward<ForwardRange>(range)),
                end(std::forward<ForwardRange>(range))
            );
    }

    /*!
        \~russian
            \brief
                Функция для создания итератора на конец подпоследовательностей

            \param end_tag
                Индикатор создания итератора на конец подпоследовательностей.
            \param begin
                Итератор на начало подпоследовательностей.

            \returns
                Итератор на пустую подпоследовательность входного диапазона.
     */
    template <typename ForwardIterator>
    auto
        make_subsequence_iterator
        (
            iterator::end_tag_t /*end_tag*/,
            const subsequence_iterator<ForwardIterator> & begin
        )
    {
        return subsequence_iterator<ForwardIterator>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST__ITERATOR__SUBSEQUENCE_ITERATOR_HPP
