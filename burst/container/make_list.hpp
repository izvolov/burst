#ifndef BURST_CONTAINER_MAKE_LIST_HPP
#define BURST_CONTAINER_MAKE_LIST_HPP

#include <burst/container/make_sequence_container.hpp>

#include <initializer_list>
#include <list>
#include <utility>

namespace burst
{
    //!     Создать std::list из списка инициализации
    /*!
            Принимает std::initializer_list и дополнительные параметры (например, аллокатор),
        выводит тип нужного списка, создаёт этот список и возвращает его.
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_list (std::initializer_list<Value> values, Xs && ... xs)
    {
        return make_sequence_container<std::list>(values, std::forward<Xs>(xs)...);
    }

    //!     Конструктор списка без явного указания типа
    /*!
            Принимает произвольные аргументы, выводит тип значения и конструирует список.
            Подробнее см. `make_sequence_container`.
     */
    template <typename ... Xs>
    auto make_list (Xs && ... xs)
    {
        return make_sequence_container<std::list>(std::forward<Xs>(xs)...);
    }

    //!     Конструктор списка с явным указанием типа
    /*!
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_list (Xs && ... xs)
    {
        return make_sequence_container<std::list, Value>(std::forward<Xs>(xs)...);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_LIST_HPP
