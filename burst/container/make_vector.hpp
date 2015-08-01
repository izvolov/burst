#ifndef BURST_CONTAINER_MAKE_VECTOR_HPP
#define BURST_CONTAINER_MAKE_VECTOR_HPP

#include <initializer_list>
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
            Отличается наличием аллокатора, передаваемого в качестве аргумента
        функции.
     */
    template <typename Value, typename Allocator>
    auto make_vector (std::initializer_list<Value> values, const Allocator & allocator)
    {
        return std::vector<Value, Allocator>(values, allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_VECTOR_HPP
