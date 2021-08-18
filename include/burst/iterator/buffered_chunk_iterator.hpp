#ifndef BURST__ITERATOR__BUFFERED_CHUNK_ITERATOR_HPP
#define BURST__ITERATOR__BUFFERED_CHUNK_ITERATOR_HPP

#include <burst/algorithm/copy_at_most_n.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>

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
            boost::iterator_range<iterator_value_t<Iterator> *>,
            std::input_iterator_tag,
            boost::iterator_range<iterator_value_t<Iterator> *>,
            iterator_difference_t<Iterator>
        >
    {
    private:
        using buffer_value_type = iterator_value_t<Iterator>;
        using base_type =
            boost::iterator_facade
            <
                buffered_chunk_iterator,
                boost::iterator_range<buffer_value_type *>,
                std::input_iterator_tag,
                boost::iterator_range<buffer_value_type *>,
                iterator_difference_t<Iterator>
            >;

    public:
        using reference = typename base_type::reference;
        using difference_type = typename base_type::difference_type;

        buffered_chunk_iterator (Iterator first, Iterator last, Integral chunk_size):
            m_current(first),
            m_end(last),
            m_chunk_size(static_cast<difference_type>(chunk_size)),
            m_chunk
            (
                std::shared_ptr<buffer_value_type>
                (
                    new buffer_value_type[static_cast<std::size_t>(m_chunk_size)],
                    std::default_delete<buffer_value_type[]>{}
                )
            ),
            m_chunk_end(m_chunk.get())
        {
            assert(m_chunk_size > 0);
            increment();
        }

        buffered_chunk_iterator (iterator::end_tag_t, const buffered_chunk_iterator & begin):
            m_current(begin.m_end),
            m_end(begin.m_end),
            m_chunk_size(begin.m_chunk_size),
            m_chunk{begin.m_chunk},
            m_chunk_end(m_chunk.get())
        {
        }

    private:
        friend class boost::iterator_core_access;

        bool empty () const
        {
            return m_chunk.get() == m_chunk_end;
        }

        reference dereference () const
        {
            return boost::make_iterator_range(m_chunk.get(), m_chunk_end);
        }

        bool equal (const buffered_chunk_iterator & that) const
        {
            assert(this->m_end == that.m_end);
            assert(this->m_chunk == that.m_chunk);
            assert(this->m_chunk_size == that.m_chunk_size);
            return this->m_current == that.m_current && empty();
        }

        void increment ()
        {
            std::tie(m_current, std::ignore, m_chunk_end) =
                copy_at_most_n(m_current, m_end, m_chunk_size, m_chunk.get());
        }

        Iterator m_current;
        Iterator m_end;
        difference_type m_chunk_size;
        std::shared_ptr<buffer_value_type> m_chunk;
        buffer_value_type * m_chunk_end;
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

#endif // BURST__ITERATOR__BUFFERED_CHUNK_ITERATOR_HPP
