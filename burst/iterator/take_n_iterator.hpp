#ifndef BURST_ITERATOR_TAKE_N_ITERATOR_HPP
#define BURST_ITERATOR_TAKE_N_ITERATOR_HPP

#include <iterator>
#include <type_traits>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <burst/iterator/end_tag.hpp>

namespace burst
{
    //!     Итератор, "откусывающий" от диапазона.
    /*!
            Присасывается к любому итератору I и откусывает первые n элементов, то есть назначает
        ему новый конец при помощи счётчика n. Иначе говоря, новый конец входного итератора I будет
        ровно I + n.

            Ответственность за допустимое продвижение итератора в данном случае лежит на
        пользователе. Это значит, что, если настоящий конец итератора I находится ближе, чем I + n,
        то такая ситуация влечёт за собой неопределённое поведение.

            Категория этого итератора — минимум из однонаправленной и категории входного итератора.
     */
    template <typename Iterator>
    class take_n_iterator:
        public boost::iterator_facade
        <
            take_n_iterator<Iterator>,
            typename std::iterator_traits<Iterator>::value_type,
            typename std::common_type
            <
                boost::forward_traversal_tag,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type
            >
            ::type,
            typename std::iterator_traits<Iterator>::reference,
            typename std::iterator_traits<Iterator>::difference_type
        >
    {
    private:
        using base_type = boost::iterator_facade
        <
            take_n_iterator,
            typename std::iterator_traits<Iterator>::value_type,
            typename std::common_type
            <
                boost::forward_traversal_tag,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type
            >
            ::type,
            typename std::iterator_traits<Iterator>::reference,
            typename std::iterator_traits<Iterator>::difference_type
        >;

    public:
        take_n_iterator (Iterator iterator, typename base_type::difference_type items_to_take):
            m_iterator(iterator),
            m_remaining(items_to_take)
        {
        }

        take_n_iterator ():
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

        typename base_type::difference_type distance_to (const take_n_iterator & that) const
        {
            return this->m_remaining - that.m_remaining;
        }

        typename base_type::reference dereference () const
        {
            return *m_iterator;
        }

        bool equal (const take_n_iterator & that) const
        {
            return this->m_remaining == that.m_remaining;
        }

    private:
        Iterator m_iterator;
        typename base_type::difference_type m_remaining;
    };

    //!     Функция для создания начала откусанного диапазона.
    /*!
            Принимает итератор и число.
            Возвращает новый итератор, конец которого равен std::next(iterator, n).

            Асимптотика: O(1) по времени и памяти.
     */
    template <typename Iterator>
    take_n_iterator<Iterator>
        make_take_n_iterator
        (
            Iterator iterator,
            typename std::iterator_traits<Iterator>::difference_type n
        )
    {
        return take_n_iterator<Iterator>(iterator, n);
    }

    //!     Функция для создания конца откусанного диапазона.
    /*!
            Принимает на вход экземпляр откусывающего итератора, формально задающего начало
        диапазона. На самом деле этот аргумент не используется, а нужен только для вывода типа
        итератора.
            Возвращает итератор на конец откусанного диапазона.

            Асимптотика: O(1) по времени и памяти.
     */
    template <typename Iterator>
    take_n_iterator<Iterator>
        make_take_n_iterator
        (
            const take_n_iterator<Iterator> &,
            iterator::end_tag_t
        )
    {
        return take_n_iterator<Iterator>{};
    }
} // namespace burst

#endif // BURST_ITERATOR_TAKE_N_ITERATOR_HPP
