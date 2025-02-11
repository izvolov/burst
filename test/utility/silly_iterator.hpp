#ifndef BURST_TEST__UTILITY__SILLY_ITERATOR_HPP
#define BURST_TEST__UTILITY__SILLY_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

#include <cstdint>
#include <memory>

namespace utility
{
    /*!
        \brief
            Дурацкий итератор

        \details
            Сущность с интерфейсом итератора произвольного доступа.
            Умеет продвигаться на любое расстояние и правильно определять расстояние между двумя
            итераторами, но при разыменовании всегда возвращает одно и то же значение.
     */
    template <typename Value>
    struct silly_iterator:
        public
            boost::iterator_facade
            <
                silly_iterator<Value>,
                Value,
                boost::random_access_traversal_tag,
                Value &,
                std::int64_t
            >
    {
    private:
        using base_type =
            boost::iterator_facade
            <
                silly_iterator<Value>,
                Value,
                boost::random_access_traversal_tag,
                Value &,
                std::int64_t
            >;

    public:
        silly_iterator ():
            m_pos(0),
            m_value(std::make_unique<Value>(0))
        {
        }

        explicit silly_iterator (std::int64_t pos):
            m_pos(pos),
            m_value(std::make_unique<Value>(0))
        {
        }

        silly_iterator (const silly_iterator & that):
            m_pos(that.m_pos),
            m_value(std::make_unique<Value>(0))
        {
        }

        silly_iterator & operator = (const silly_iterator & that)
        {
            m_pos = that.m_pos;
            return *this;
        }

        ~silly_iterator () = default;

        void increment ()
        {
            ++m_pos;
        }

        void decrement ()
        {
            --m_pos;
        }

        void advance (typename base_type::difference_type n)
        {
            m_pos += n;
        }

        bool equal (const silly_iterator & that) const
        {
            return this->m_pos == that.m_pos;
        }

        typename base_type::difference_type distance_to (const silly_iterator & that) const
        {
            return that.m_pos - this->m_pos;
        }

        typename base_type::reference dereference () const
        {
            return *m_value;
        }

    private:
        std::int64_t m_pos;
        std::unique_ptr<Value> m_value;
    };
}

#endif // BURST_TEST__UTILITY__SILLY_ITERATOR_HPP
