#ifndef BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP
#define BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP

#include <burst/concept/check.hpp>
#include <burst/concept/integer.hpp>

#include <boost/range/value_type.hpp>

#include <initializer_list>
#include <iterator>
#include <utility>

namespace burst
{
    //!     Создать последовательний контейнер без явного указания типа его значений
    /*!
            Принимает std::initializer_list, из типа его значений выводит тип нужного контейнера,
        создаёт этот контейнер и возвращает его.
     */
    template <template <typename ...> class SequenceContainer, typename Value>
    auto make_sequence_container (std::initializer_list<Value> values)
    {
        return SequenceContainer<Value>(values);
    }

    //!     Создание контейнера с пользовательским аллокатором
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции
     */
    template <template <typename ...> class SequenceContainer, typename Value, typename Allocator>
    auto make_sequence_container (std::initializer_list<Value> values, const Allocator & allocator)
    {
        return SequenceContainer<Value, Allocator>(values, allocator);
    }

    //!     Создать последовательний контейнер из диапазона
    /*!
            Принимает произвольный диапазон, из типа его значений выводит тип элементов контейнера,
        создаёт этот контейнер и возвращает его.
     */
    template <template <typename ...> class SequenceContainer, typename InputRange>
    auto make_sequence_container (InputRange && values)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        using std::begin;
        using std::end;
        return
            SequenceContainer<value_type>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values))
            );
    }

    //!     Создать последовательний контейнер из диапазона и аллокатора
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.

            `make_sequence_container(range, allocator)`
     */
    template <template <typename ...> class SequenceContainer, typename InputRange, typename Allocator,
        typename = Not<Integer, InputRange>>
    auto make_sequence_container (InputRange && values, const Allocator & allocator)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        using std::begin;
        using std::end;
        return
            SequenceContainer<value_type, Allocator>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values)),
                allocator
            );
    }

    //!     Создать последовательний контейнер нужного размера, заполнив копиями значения
    /*!
            Принимает размер контейнера, а также произвольное значение, тип которого и будет
        являться типом элементов контейнера. Создаёт контейнер заданного размера, каждый элемент
        которого будет копией заданного элемента.

            `make_sequence_container(5, value)`
     */
    template <template <typename ...> class SequenceContainer, typename I, typename Value>
    auto make_sequence_container (Integer<I> size, const Value & value)
    {
        return SequenceContainer<Value>(size, value);
    }

    //!     Создать последовательний контейнер из диапазона с явным указанием типа его значений
    /*!
            Отличается тем, что тип значений контейнера не выводится из типа значений диапазона, а
        указывается пользователем явно.

            `make_sequence_container<std::list, std::uint32_t>(range)`
     */
    template <template <typename ...> class SequenceContainer, typename Value, typename InputRange>
    auto make_sequence_container (InputRange && values)
    {
        using std::begin;
        using std::end;
        return
            SequenceContainer<Value>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values))
            );
    }

    //!     Создать последовательний контейнер из диапазона с аллокатором и явным указанием типа его значений
    /*!
            Отличается тем, что тип значений контейнера не выводится из типа значений диапазона, а
        указывается пользователем явно.

            `make_sequence_container<std::vector, std::uint32_t>(range, allocator)`
     */
    template
    <
        template <typename ...> class SequenceContainer,
        typename Value,
        typename InputRange,
        typename Allocator
    >
    auto make_sequence_container (InputRange && values, const Allocator & allocator)
    {
        using std::begin;
        using std::end;
        return
            SequenceContainer<Value, Allocator>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values)),
                allocator
            );
    }

    //!     Создать последовательный контейнер из диапазона, заданного двумя итераторами
    /*!
            Тип элементов контейнера выводится из типа значений итератора.
     */
    template <template <typename ...> class SequenceContainer, typename InputIterator>
    auto make_sequence_container (InputIterator first, InputIterator last)
    {
        using value_type = typename std::iterator_traits<InputIterator>::value_type;
        return SequenceContainer<value_type>(first, last);
    }

    //!     Создать последовательный контейнер из диапазона, заданного двумя итераторами, с аллокатором
    /*!
            Тип элементов контейнера выводится из типа значений итератора.
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template
    <
        template <typename ...> class SequenceContainer,
        typename InputIterator,
        typename Allocator
    >
    auto make_sequence_container (InputIterator first, InputIterator last, const Allocator & allocator)
    {
        using value_type = typename std::iterator_traits<InputIterator>::value_type;
        return SequenceContainer<value_type, Allocator>(first, last, allocator);
    }

    //!     Создать последовательный контейнер из двух итераторов с явным указанием типа значений
    /*!
            Отличается тем, что тип значений контейнера не выводится из типа значений итератора, а
        указывается пользователем явно.
     */
    template
    <
        template <typename ...> class SequenceContainer,
        typename Value,
        typename InputIterator
    >
    auto make_sequence_container (InputIterator first, InputIterator last)
    {
        return SequenceContainer<Value>(first, last);
    }

    //!     Создать последовательный контейнер из двух итераторов и аллокатора с явным указанием типа значений
    /*!
            Отличается наличием аллокатора и явным заданием типа значений результирующего
        контейнера.
     */
    template
    <
        template <typename ...> class SequenceContainer,
        typename Value,
        typename InputIterator,
        typename Allocator
    >
    auto make_sequence_container (InputIterator first, InputIterator last, const Allocator & allocator)
    {
        return SequenceContainer<Value, Allocator>(first, last, allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP
