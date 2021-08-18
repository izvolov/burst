#ifndef BURST__ITERATOR__BINARY_ISTREAM_ITERATOR_HPP
#define BURST__ITERATOR__BINARY_ISTREAM_ITERATOR_HPP

#include <burst/functional/trivial_read.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>

#include <istream>
#include <memory>
#include <utility>

namespace burst
{
    //!     Итератор бинарного ввода
    /*!
            Предназначен для считывания из потока последовательности объектов, записанных в
        бинарном виде.

        \tparam Value
            Тип объектов последовательности.
        \tparam Read
            Функция, с помощью которой будет производиться чтение каждого объекта.
            Обязана иметь следующий интерфейс:

                std::istream & (std::istream & stream, Value & value)

            В случае, если прочитать объект успешно удалось, он записывается в переменную `value`.
     */
    template <typename Value, typename Read>
    class binary_istream_iterator:
        public
            boost::iterator_facade
            <
                binary_istream_iterator<Value, Read>,
                Value,
                boost::single_pass_traversal_tag,
                const Value &
            >
    {
    private:
        using base_type =
            boost::iterator_facade
            <
                binary_istream_iterator,
                Value,
                boost::single_pass_traversal_tag,
                const Value &
            >;

    public:
        binary_istream_iterator (std::istream & stream, Read read):
            m_value{},
            m_stream(std::addressof(stream)),
            m_read(std::move(read)),
            m_ok(try_to_read())
        {
        }

        binary_istream_iterator ():
            m_value{},
            m_stream(nullptr),
            m_read{},
            m_ok(false)
        {
        }

        bool try_to_read ()
        {
            return static_cast<bool>((*m_read)(*m_stream, m_value));
        }

        void increment ()
        {
            m_ok = try_to_read();
        }

        bool equal (const binary_istream_iterator & that) const
        {
            return this->m_ok == that.m_ok;
        }

        typename base_type::reference dereference () const
        {
            return m_value;
        }

    private:
        Value m_value;
        std::istream * m_stream;
        boost::optional<Read> m_read;
        bool m_ok;
    };

    template <typename Value, typename Read = trivial_read_fn>
    auto make_binary_istream_iterator (std::istream & stream, Read read = Read{})
    {
        return binary_istream_iterator<Value, Read>(stream, std::move(read));
    }

    template <typename Value, typename Read>
    auto
        make_binary_istream_iterator
        (
            iterator::end_tag_t,
            const binary_istream_iterator<Value, Read> &
        )
    {
        return binary_istream_iterator<Value, Read>{};
    }
} // namespace burst

#endif // BURST__ITERATOR__BINARY_ISTREAM_ITERATOR_HPP
