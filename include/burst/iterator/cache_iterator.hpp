#ifndef BURST__ITERATOR__CACHE_ITERATOR_HPP
#define BURST__ITERATOR__CACHE_ITERATOR_HPP

#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace burst
{
    //!     Кэширующий итератор
    /*!
            Нужен для того, чтобы хранить результат промежуточного вычисления при работе с
        итераторами и диапазонами.

            Натравливается на итератор, который при разыменовании возвращает по значению, и
        запоминает этот итератор.
            При каждом разыменовании смотрит, было ли уже вычисленно значение, которое возвращает
        обёрнутый итератор. Если нет, то разыменовывает обёрнутый итератор и запоминает
        возвращённое значение. Далее просто возвращает вычисленное значение по ссылке.

            При любом продвижении выбрасывает вычисленное значение.

            Не изменяет тип обхода обёрнутого итератора. Иными словами, если обёрнутый итератор
        однонаправленный, то и кэширующий итератор будет однонаправленным, если обёрнутый
        произвольного доступа, то и кэширующий будет произвольного доступа, и т.п.
     */
    template <typename Iterator>
    struct cache_iterator:
        public
            boost::iterator_facade
            <
                cache_iterator<Iterator>,
                iterator_value_t<Iterator>,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type,
                const iterator_value_t<Iterator> &,
                iterator_difference_t<Iterator>
            >
    {
    private:
        static_assert(not std::is_lvalue_reference<iterator_reference_t<Iterator>>::value, "");

        using base_type =
            boost::iterator_facade
            <
                cache_iterator<Iterator>,
                iterator_value_t<Iterator>,
                typename boost::iterators::pure_iterator_traversal<Iterator>::type,
                const iterator_value_t<Iterator> &,
                iterator_difference_t<Iterator>
            >;
    public:
        explicit cache_iterator (Iterator iterator):
            m_iterator(std::move(iterator))
        {
        }

        void increment ()
        {
            m_cached_value = boost::none;
            ++m_iterator;
        }

        void decrement ()
        {
            m_cached_value = boost::none;
            --m_iterator;
        }

        void advance (typename base_type::difference_type n)
        {
            m_cached_value = boost::none;
            std::advance(m_iterator, n);
        }

        bool equal (const cache_iterator & that) const
        {
            return this->m_iterator == that.m_iterator;
        }

        typename base_type::difference_type distance_to (const cache_iterator & that) const
        {
            return that.m_iterator - this->m_iterator;
        }

        typename base_type::reference dereference () const
        {
            if (m_cached_value == boost::none)
            {
                m_cached_value = *m_iterator;
            }

            return *m_cached_value;
        }

    private:
        Iterator m_iterator;
        mutable boost::optional<typename base_type::value_type> m_cached_value;
    };

    //!     Создание кэширующего итератора
    /*!
            Специализация для случая, когда оборачиваемый итератор возвращает по ссылке.
            Тот факт, что значение возвращается по ссылке означает, что кто-то уже владеет этим
        значением, поэтому не надо создавать никакой обёртки, а нужно просто пробросить входной
        итератор обратно наружу.
     */
    template <typename Iterator>
    typename std::enable_if
    <
        std::is_lvalue_reference<iterator_reference_t<Iterator>>::value,
        Iterator
    >
    ::type make_cache_iterator (Iterator iterator)
    {
        return iterator;
    }

    //!     Создание кэширующего итератора
    /*!
            Специализация для случая, когда оборачиваемый итератор возвращает по значению.
            Создаёт кэширующий итератор, кладя в него поданный на вход итератор.
     */
    template <typename Iterator>
    typename std::enable_if
    <
        not std::is_lvalue_reference<iterator_reference_t<Iterator>>::value,
        cache_iterator<Iterator>
    >
    ::type make_cache_iterator (Iterator iterator)
    {
        return cache_iterator<Iterator>(std::move(iterator));
    }
}

#endif // BURST__ITERATOR__CACHE_ITERATOR_HPP
