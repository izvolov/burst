#ifndef BURST_CONTAINER_MAKE_FORWARD_LIST_HPP
#define BURST_CONTAINER_MAKE_FORWARD_LIST_HPP

#include <forward_list>
#include <initializer_list>

namespace burst
{
    //!     Создать std::forward_list без явного указания типа его значений.
    /*!
            Принимает std::initializer_list, из типа его значений выводит тип нужного списка,
        создаёт этот список и возвращает его.
     */
    template <typename Value>
    auto make_forward_list (std::initializer_list<Value> values)
    {
        return std::forward_list<Value>(values);
    }

    //!     Создание односвязного списка с пользовательским аллокатором.
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename Value, typename Allocator>
    auto make_forward_list (std::initializer_list<Value> values, const Allocator & allocator)
    {
        return std::forward_list<Value, Allocator>(values, allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_FORWARD_LIST_HPP
