#ifndef BURST_CONTAINER_MAKE_LIST_HPP
#define BURST_CONTAINER_MAKE_LIST_HPP

#include <initializer_list>
#include <list>

namespace burst
{
    //!     Создать std::list без явного указания типа его значений.
    /*!
            Принимает std::initializer_list, из типа его значений выводит тип нужного списка,
        создаёт этот список и возвращает его.
     */
    template <typename Value>
    auto make_list (std::initializer_list<Value> values)
    {
        return std::list<Value>(values);
    }

    //!     Создание списка с пользовательским аллокатором.
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename Value, typename Allocator>
    auto make_list (std::initializer_list<Value> values, const Allocator & allocator)
    {
        return std::list<Value, Allocator>(values, allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_LIST_HPP
