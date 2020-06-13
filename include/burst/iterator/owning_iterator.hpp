#ifndef BURST_ITERATOR_OWNING_ITERATOR_HPP
#define BURST_ITERATOR_OWNING_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace burst
{
    //!     Итератор, владеющий контейнером
    /*!
            Хранит итерируемый контейнер и итератор этого контейнера. Время жизни итерируемого
        контейнера ограничего созданием первого и уничтожением последнего экземпляров владеющего
        итератора. Таким образом, нет необходимости хранить этот контейнер где-то снаружи.
            Сохраняет все характеристики внутреннего итератора. Все действия с ним — продвижение,
        разыменование, сравнение — производятся так, как будто они производятся с внутренним
        итератором.
     */
    template <typename Container>
    struct owning_iterator:
        public
            boost::iterator_facade
            <
                owning_iterator<Container>,
                iterator_value_t<typename Container::iterator>,
                typename boost::iterators::pure_iterator_traversal<typename Container::iterator>::type,
                iterator_reference_t<typename Container::iterator>,
                iterator_difference_t<typename Container::iterator>
            >
    {
    private:
        using iterator = typename Container::iterator;
        using base_type =
            boost::iterator_facade
            <
                owning_iterator<Container>,
                iterator_value_t<iterator>,
                typename boost::iterators::pure_iterator_traversal<iterator>::type,
                iterator_reference_t<iterator>,
                iterator_difference_t<iterator>
            >;

    public:
        explicit owning_iterator (Container && container):
            m_container(std::make_shared<Container>(std::move(container))),
            m_iterator(m_container->begin())
        {
        }

        owning_iterator (burst::iterator::end_tag_t, const owning_iterator & begin):
            m_container(begin.m_container),
            m_iterator(m_container->end())
        {
        }

        owning_iterator () = default;

        void increment ()
        {
            ++m_iterator;
        }

        void decrement ()
        {
            --m_iterator;
        }

        void advance (typename base_type::difference_type n)
        {
            std::advance(m_iterator, n);
        }

        bool equal (const owning_iterator & that) const
        {
            assert(this->m_container == that.m_container);
            return this->m_iterator == that.m_iterator;
        }

        typename base_type::difference_type distance_to (const owning_iterator & that) const
        {
            assert(this->m_container == that.m_container);
            return that.m_iterator - this->m_iterator;
        }

        typename base_type::reference dereference () const
        {
            return *m_iterator;
        }

    private:
        std::shared_ptr<Container> m_container;
        iterator m_iterator;
    };

    //!     Создание владеющего итератора
    /*!
            Принимает контейнер, который нужно проитерировать. Запоминает этот контейнер и
        возвращает владеющий итератор, соответствующий итератору на начало этого контейнера.
     */
    template <typename Container>
    auto make_owning_iterator (Container && container)
    {
        return owning_iterator<std::decay_t<Container>>(std::forward<Container>(container));
    }

    //!     Создание владеющего итератора на конец
    /*!
            Принимает на вход владеющий итератор на начало и специальный индикатор, показывающий,
        что нужно создать итератор на конец.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        контейнера закончились.
     */
    template <typename Container>
    auto make_owning_iterator (iterator::end_tag_t, const owning_iterator<Container> & begin)
    {
        return owning_iterator<Container>(iterator::end_tag, begin);
    }
}

#endif // BURST_ITERATOR_OWNING_ITERATOR_HPP
