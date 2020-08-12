#ifndef BURST_ITERATOR_BUFFERED_CHUNK_ITERATOR_HPP
#define BURST_ITERATOR_BUFFERED_CHUNK_ITERATOR_HPP

#include <burst/algorithm/copy_at_most_n.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/iterator/iterator_facade.hpp>

#include <cassert>
#include <iterator>
#include <memory>
#include <tuple>
#include <vector>

namespace burst
{
    /*!
        \brief
            Буферизованный кусочный итератор

        \details
            Разбивает входящую последовательность на равные куски (последний кусок может быть
            меньше, если элементов не хватает до целого) заданного размера.
            Текущий кусок хранится в буфере внутри итератора.

            Моделирует однопроходный итератор (InputIterator).

        \tparam Iterator
            Тип исходного итератора.
        \tparam Integral
            Тип числа, с помощью которого будет задаваться размер одного куска.

        \see make_buffered_chunk_iterator
     */
    template <typename Iterator, typename Integral>
    class buffered_chunk_iterator:
        public boost::iterator_facade
        <
            buffered_chunk_iterator<Iterator, Integral>,
            std::vector<iterator_value_t<Iterator>>,
            std::input_iterator_tag,
            std::vector<iterator_value_t<Iterator>> &,
            iterator_difference_t<Iterator>
        >
    {
    private:
        using container_type = std::vector<iterator_value_t<Iterator>>;
        using base_type =
            boost::iterator_facade
            <
                buffered_chunk_iterator,
                container_type,
                std::input_iterator_tag,
                container_type &,
                iterator_difference_t<Iterator>
            >;

    public:
        using reference = typename base_type::reference;
        using difference_type = typename base_type::difference_type;

        buffered_chunk_iterator (Iterator first, Iterator last, Integral chunk_size):
            m_current(first),
            m_end(last),
            m_chunk(std::make_shared<container_type>()),
            m_chunk_size(static_cast<difference_type>(chunk_size))
        {
            assert(m_chunk_size > 0);

            m_chunk->reserve(static_cast<typename container_type::size_type>(m_chunk_size));
            increment();
        }

        buffered_chunk_iterator (iterator::end_tag_t, const buffered_chunk_iterator & begin):
            m_current(begin.m_end),
            m_end(begin.m_end),
            m_chunk{begin.m_chunk},
            m_chunk_size(begin.m_chunk_size)
        {
        }

    private:
        friend class boost::iterator_core_access;

        reference dereference () const
        {
            return *m_chunk;
        }

        bool equal (const buffered_chunk_iterator & that) const
        {
            assert(this->m_end == that.m_end);
            assert(this->m_chunk == that.m_chunk);
            assert(this->m_chunk_size == that.m_chunk_size);
            return this->m_current == that.m_current && this->m_chunk->empty();
        }

        void increment ()
        {
            m_chunk->clear();
            std::tie(m_current, std::ignore, std::ignore) =
                copy_at_most_n(m_current, m_end, m_chunk_size, std::back_inserter(*m_chunk));
        }

        Iterator m_current;
        Iterator m_end;
        std::shared_ptr<container_type> m_chunk;
        difference_type m_chunk_size;
    };

    /*!
        \brief
            Функция для создания буферизованного кусочного итератора

        \param [first, last)
            Последовательность, которая будет побита на куски.
        \param n
            Размер куска.

        \returns
            Экземпляр буферизованного кусочного итератора.

        \see buffered_chunk_iterator
     */
    template <typename Iterator, typename Integral>
    auto make_buffered_chunk_iterator (Iterator first, Iterator last, Integral n)
    {
        return buffered_chunk_iterator<Iterator, Integral>(first, last, n);
    }

    /*!
        \brief
            Функция для создания буферизованного кусочного итератора

        \param range
            Последовательность, которая будет побита на куски.
        \param n
            Размер куска.

        \warning
            Время жизни диапазона `range` должно быть не меньше времени жизни полученного
            буферизованного кусочного итератора, в противном случае поведение неопределено.

        \returns
            Экземпляр буферизованного кусочного итератора.

        \see buffered_chunk_iterator
     */
    template <typename Range, typename Integral>
    auto make_buffered_chunk_iterator (const Range & range, Integral n)
    {
        using std::begin;
        using std::end;
        return make_buffered_chunk_iterator(begin(range), end(range), n);
    }

    /*!
        \brief
            Функция для создания конца буферизованного кусочного диапазона

        \details
            Принимает на вход итератор на начало кусочного диапазона и специальную метку,
            обозначающую, что нужно создать итератор на конец диапазона.

        \returns
            Итератор на конец буферизованного кусочного диапазона.

        \see buffered_chunk_iterator
     */
    template <typename Iterator, typename Integral>
    auto
        make_buffered_chunk_iterator
        (
            iterator::end_tag_t,
            const buffered_chunk_iterator<Iterator, Integral> & begin
        )
    {
        return buffered_chunk_iterator<Iterator, Integral>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_BUFFERED_CHUNK_ITERATOR_HPP
