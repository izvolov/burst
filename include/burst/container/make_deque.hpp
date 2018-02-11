#ifndef BURST_CONTAINER_MAKE_DEQUE_HPP
#define BURST_CONTAINER_MAKE_DEQUE_HPP

#include <burst/container/make_sequence_container.hpp>

#include <deque>
#include <initializer_list>
#include <utility>

namespace burst
{
    //!     Создать std::deque из списка инициализации
    /*!
            Принимает std::initializer_list и дополнительные параметры (например, аллокатор),
        выводит тип нужной двусторонней очереди, создаёт эту очередь и возвращает её.
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_deque (std::initializer_list<Value> values, Xs && ... xs)
    {
        return make_sequence_container<std::deque>(values, std::forward<Xs>(xs)...);
    }

    //!     Конструктор двусторонней очереди без явного указания типа
    /*!
            Принимает произвольные аргументы, выводит тип значения и конструирует очередь.
            Подробнее см. `make_sequence_container`.
     */
    template <typename ... Xs>
    auto make_deque (Xs && ... xs)
    {
        return make_sequence_container<std::deque>(std::forward<Xs>(xs)...);
    }

    //!     Конструктор двусторонней очереди с явным указанием типа
    /*!
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_deque (Xs && ... xs)
    {
        return make_sequence_container<std::deque, Value>(std::forward<Xs>(xs)...);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_DEQUE_HPP
