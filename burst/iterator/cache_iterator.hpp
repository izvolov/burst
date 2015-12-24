#ifndef BURST_ITERATOR_CACHE_ITERATOR_HPP
#define BURST_ITERATOR_CACHE_ITERATOR_HPP

#include <iterator>
#include <type_traits>
#include <utility>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>

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
                typename std::iterator_traits<Iterator>::value_type,
                typename boost::iterators::pure_traversal_tag
                <
                    typename boost::iterator_traversal<Iterator>::type
                >
                ::type,
                const typename std::iterator_traits<Iterator>::value_type &,
                typename std::iterator_traits<Iterator>::difference_type
            >
    {
    private:
        static_assert(not std::is_lvalue_reference<typename std::iterator_traits<Iterator>::reference>::value, "");

        using base_type =
            boost::iterator_facade
            <
                cache_iterator<Iterator>,
                typename std::iterator_traits<Iterator>::value_type,
                typename boost::iterators::pure_traversal_tag
                <
                    typename boost::iterator_traversal<Iterator>::type
                >
                ::type,
                const typename std::iterator_traits<Iterator>::value_type &,
                typename std::iterator_traits<Iterator>::difference_type
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
        std::is_lvalue_reference
        <
            typename std::iterator_traits<Iterator>::reference
        >
        ::value,
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
        not std::is_lvalue_reference
        <
            typename std::iterator_traits<Iterator>::reference
        >
        ::value,
        cache_iterator<Iterator>
    >
    ::type make_cache_iterator (Iterator iterator)
    {
        return cache_iterator<Iterator>(std::move(iterator));
    }
}

#endif // BURST_ITERATOR_CACHE_ITERATOR_HPP
