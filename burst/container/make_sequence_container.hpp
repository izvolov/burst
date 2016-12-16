#ifndef BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP
#define BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP

#include <burst/container/detail/make_sequence_container.hpp>

#include <boost/range/value_type.hpp>

#include <initializer_list>
#include <iterator>
#include <type_traits>
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
        return
            SequenceContainer<value_type>
            (
                std::begin(std::forward<InputRange>(values)),
                std::end(std::forward<InputRange>(values))
            );
    }

    //!     Создать последовательний контейнер из двух аргументов
    /*!
            Включает в себя два случая:

            1. Аргументы — диапазон и аллокатор.

                Тип элементов контейнера выводится из значений диапазона и вызывается конструктор
                от диапазона и аллокатора:

                `SequenceContainer<range_value<R>>(begin(range), end(range), allocator)`

            2. Аргументы — целое число и произвольное значение.

                Тип элементов контейнера выводится из типа второго аргумента и вызывается
                конструктор:

                `SequenceContainer<V>(size, value)`
     */
    template <template <typename ...> class SequenceContainer, typename First, typename Second>
    auto make_sequence_container (First && first, Second && second)
    {
        return
            detail::make_sequence_container_impl<SequenceContainer>
            (
                std::forward<First>(first), std::forward<Second>(second),
                std::is_integral<std::decay_t<First>>{}
            );
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
        return
            SequenceContainer<Value>
            (
                std::begin(std::forward<InputRange>(values)),
                std::end(std::forward<InputRange>(values))
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
        return
            SequenceContainer<Value, Allocator>
            (
                std::begin(std::forward<InputRange>(values)),
                std::end(std::forward<InputRange>(values)),
                allocator
            );
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_SEQUENCE_CONTAINER_HPP
