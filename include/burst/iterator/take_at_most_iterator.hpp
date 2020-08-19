#ifndef BURST_ITERATOR_TAKE_AT_MOST_ITERATOR_HPP
#define BURST_ITERATOR_TAKE_AT_MOST_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_category.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/minimum_category.hpp>
#include <burst/type_traits/range_iterator.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <iterator>

namespace burst
{
    /*!
        \brief
            Итератор, "откусывающий" от диапазона не более n элементов

        \details
            Присасывается к любому итератору `I` и откусывает либо первые `n` элементов, либо
            столько элементов, сколько есть в диапазоне `[I, J)`, где `J` — некоторый достижимый
            из `I` итератор, заданный пользователем.

            Категория этого итератора — минимум из однонаправленной и категории входного итератора.
     */
    template <typename Iterator, typename Integer>
    class take_at_most_iterator:
        public boost::iterator_facade
        <
            take_at_most_iterator<Iterator, Integer>,
            iterator_value_t<Iterator>,
            minimum_category_t<std::forward_iterator_tag, iterator_category_t<Iterator>>,
            iterator_reference_t<Iterator>,
            iterator_difference_t<Iterator>
        >
    {
    private:
        using base_type =
            boost::iterator_facade
            <
                take_at_most_iterator,
                iterator_value_t<Iterator>,
                minimum_category_t<std::forward_iterator_tag, iterator_category_t<Iterator>>,
                iterator_reference_t<Iterator>,
                iterator_difference_t<Iterator>
            >;

    public:
        take_at_most_iterator (Iterator first, Iterator last, Integer items_to_take):
            m_begin(first),
            m_end(last),
            m_remaining(items_to_take)
        {
        }

        take_at_most_iterator ():
            m_begin{},
            m_end{},
            m_remaining(0)
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            ++m_begin;
            if (m_begin == m_end)
            {
                m_remaining = Integer{0};
            }
            else
            {
                --m_remaining;
            }
        }

        typename base_type::reference dereference () const
        {
            return *m_begin;
        }

        bool equal (const take_at_most_iterator & that) const
        {
            return this->m_remaining == that.m_remaining;
        }

    private:
        Iterator m_begin;
        Iterator m_end;
        Integer m_remaining;
    };

    /*!
        \brief
            Функция для создания начала откусанного диапазона

        \details
            Асимптотика: O(1) по времени и памяти.

        \param [first, last)
            Диапазон, от которого будет откусан кусок.
        \param n
            Размер куска.

        \returns
            Экземпляр класса `take_at_most_iterator`.

        \see take_at_most_iterator
     */
    template <typename Iterator, typename Integer>
    auto make_take_at_most_iterator (Iterator first, Iterator last, Integer n)
    {
        return take_at_most_iterator<Iterator, Integer>(first, last, n);
    }

    /*!
        \brief
            Функция для создания начала откусанного диапазона

        \details
            Асимптотика: O(1) по времени и памяти.

        \param range
            Диапазон, от которого будет откусан кусок.
        \param n
            Размер куска.

        \returns
            Экземпляр класса `take_at_most_iterator`.

        \see take_at_most_iterator
     */
    template <typename Range, typename Integer>
    auto make_take_at_most_iterator (Range && r, Integer n)
    {
        using iterator = range_iterator_t<Range>;
        using std::begin;
        using std::end;
        return
            take_at_most_iterator<iterator, Integer>
            (
                begin(std::forward<Range>(r)),
                end(std::forward<Range>(r)),
                n
            );
    }

    /*!
        \brief
            Функция для создания конца откусанного диапазона

        \details
            Принимает на вход экземпляр откусывающего итератора, формально задающего начало
            диапазона. На самом деле этот аргумент не используется, а нужен только для вывода типа
            итератора.

            Асимптотика: O(1) по времени и памяти.

        \returns
            Итератор на конец откусанного диапазона.

        \see take_at_most_iterator
     */
    template <typename Iterator, typename Integer>
    auto
        make_take_at_most_iterator
        (
            iterator::end_tag_t,
            const take_at_most_iterator<Iterator, Integer> &
        )
    {
        return take_at_most_iterator<Iterator, Integer>{};
    }
} // namespace burst

#endif // BURST_ITERATOR_TAKE_AT_MOST_ITERATOR_HPP
