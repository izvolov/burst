#ifndef BURST_ITERATOR_TAKE_EXACTLY_ITERATOR_HPP
#define BURST_ITERATOR_TAKE_EXACTLY_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <type_traits>

namespace burst
{
    /*!
        \brief
            Итератор, "откусывающий" от диапазона ровно n элементов

        \details
            Присасывается к любому итератору I и откусывает первые n элементов, то есть назначает
            ему новый конец при помощи счётчика n. Иначе говоря, новый конец входного итератора I
            будет ровно I + n.

            Категория этого итератора — минимум из однонаправленной и категории входного итератора.

        \warning
            Ответственность за допустимое продвижение итератора в данном случае лежит на
            пользователе. Это значит, что, если настоящий конец итератора I находится ближе,
            чем I + n, то такая ситуация влечёт за собой неопределённое поведение.
     */
    template <typename Iterator, typename Integer>
    class take_exactly_iterator:
        public boost::iterator_facade
        <
            take_exactly_iterator<Iterator, Integer>,
            iterator_value_t<Iterator>,
            typename std::common_type
            <
                boost::forward_traversal_tag,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type
            >
            ::type,
            iterator_reference_t<Iterator>,
            iterator_difference_t<Iterator>
        >
    {
    private:
        using base_type = boost::iterator_facade
        <
            take_exactly_iterator,
            iterator_value_t<Iterator>,
            typename std::common_type
            <
                boost::forward_traversal_tag,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type
            >
            ::type,
            iterator_reference_t<Iterator>,
            iterator_difference_t<Iterator>
        >;

    public:
        take_exactly_iterator (Iterator iterator, Integer items_to_take):
            m_iterator(iterator),
            m_remaining(items_to_take)
        {
        }

        take_exactly_iterator ():
            m_iterator{},
            m_remaining(0)
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            ++m_iterator;
            --m_remaining;
        }

        typename base_type::reference dereference () const
        {
            return *m_iterator;
        }

        bool equal (const take_exactly_iterator & that) const
        {
            return this->m_remaining == that.m_remaining;
        }

    private:
        Iterator m_iterator;
        Integer m_remaining;
    };

    /*!
        \brief
            Функция для создания начала откусанного диапазона

        \details
            Асимптотика: O(1) по времени и памяти.

        \param iterator
            Итератор, от которого будет отсчитываться кусок.
        \param n
            Размер куска.

        \returns
            Новый итератор, конец которого равен std::next(iterator, n).
     */
    template <typename Iterator, typename Integer>
    auto make_take_exactly_iterator (Iterator iterator, Integer n)
    {
        return take_exactly_iterator<Iterator, Integer>(iterator, n);
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
     */
    template <typename Iterator, typename Integer>
    auto
        make_take_exactly_iterator
        (
            iterator::end_tag_t,
            const take_exactly_iterator<Iterator, Integer> &
        )
    {
        return take_exactly_iterator<Iterator, Integer>{};
    }
} // namespace burst

#endif // BURST_ITERATOR_TAKE_EXACTLY_ITERATOR_HPP
