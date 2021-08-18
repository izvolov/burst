#ifndef BURST__CONTAINER__MAKE_VECTOR_HPP
#define BURST__CONTAINER__MAKE_VECTOR_HPP

#include <burst/container/make_sequence_container.hpp>

#include <initializer_list>
#include <vector>
#include <utility>

namespace burst
{
    //!     Создать std::vector из списка инициализации
    /*!
            Принимает std::initializer_list и дополнительные параметры (например, аллокатор),
        выводит тип нужного вектора, создаёт этот вектор и возвращает его.
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_vector (std::initializer_list<Value> values, Xs && ... xs)
    {
        return make_sequence_container<std::vector>(values, std::forward<Xs>(xs)...);
    }

    //!     Конструктор вектора без явного указания типа
    /*!
            Принимает произвольные аргументы, выводит тип значения и конструирует вектор.
            Подробнее см. `make_sequence_container`.
     */
    template <typename ... Xs>
    auto make_vector (Xs && ... xs)
    {
        return make_sequence_container<std::vector>(std::forward<Xs>(xs)...);
    }

    //!     Конструктор вектора с явным указанием типа
    /*!
            Подробнее см. `make_sequence_container`.
     */
    template <typename Value, typename ... Xs>
    auto make_vector (Xs && ... xs)
    {
        return make_sequence_container<std::vector, Value>(std::forward<Xs>(xs)...);
    }
} // namespace burst

#endif // BURST__CONTAINER__MAKE_VECTOR_HPP
