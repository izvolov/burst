#ifndef BURST_CONTAINER_MAKE_VECTOR_HPP
#define BURST_CONTAINER_MAKE_VECTOR_HPP

#include <boost/range/value_type.hpp>

#include <initializer_list>
#include <iterator>
#include <vector>

namespace burst
{
    //!     Создать std::vector без явного указания типа его значений.
    /*!
            Принимает std::initializer_list, из типа его значений выводит тип нужного вектора,
        создаёт этот вектор и возвращает его.
     */
    template <typename Value>
    auto make_vector (std::initializer_list<Value> values)
    {
        return std::vector<Value>(values);
    }

    //!     Создание вектора с пользовательским аллокатором.
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename Value, typename Allocator>
    auto make_vector (std::initializer_list<Value> values, const Allocator & allocator)
    {
        return std::vector<Value, Allocator>(values, allocator);
    }

    //!     Создать std::vector из диапазона
    /*!
            Принимает произвольный диапазон, из типа его значений выводит тип элементов вектора,
        создаёт этот вектор и возвращает его.
     */
    template <typename InputRange>
    auto make_vector (InputRange && values)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        return std::vector<value_type>(std::begin(values), std::end(values));
    }

    //!     Создать std::vector из диапазона с аллокатором
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename InputRange, typename Allocator>
    auto make_vector (InputRange && values, const Allocator & allocator)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        return std::vector<value_type, Allocator>(std::begin(values), std::end(values), allocator);
    }

    //!     Создать std::vector из диапазона с явным указанием типа его значений
    /*!
            Отличается тем, что тип значений вектора не выводится из типа значений контейнера, а
        указывается пользователем явно.

            `make_vector<std::uint32_t>(range)`
     */
    template <typename Value, typename InputRange>
    auto make_vector (InputRange && values)
    {
        return std::vector<Value>(std::begin(values), std::end(values));
    }

    //!     Создать std::vector из диапазона с аллокатором и явным указанием типа его значений
    /*!
            Отличается тем, что тип значений вектора не выводится из типа значений контейнера, а
        указывается пользователем явно.

            `make_vector<std::uint32_t>(range)`
     */
    template <typename Value, typename InputRange, typename Allocator>
    auto make_vector (InputRange && values, const Allocator & allocator)
    {
        return std::vector<Value, Allocator>(std::begin(values), std::end(values), allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_VECTOR_HPP
