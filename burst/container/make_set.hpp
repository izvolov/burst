#ifndef BURST_CONTAINER_MAKE_SET_HPP
#define BURST_CONTAINER_MAKE_SET_HPP

#include <initializer_list>
#include <set>

namespace burst
{
    //!     Создать std::set без явного указания типа его значений.
    /*!
            Принимает std::initializer_list, из типа его значений выводит тип нужного множества,
        создаёт это множество и возвращает его.
     */
    template <typename Value>
    auto make_set (std::initializer_list<Value> values)
    {
        return std::set<Value>(values);
    }

    //!     Создание множества с явным заданием отношения порядка
    /*!
            Отличается наличием функции, задающей отношение порядка на элементах множества.
     */
    template <typename Value, typename Compare>
    auto make_set (std::initializer_list<Value> values, const Compare & compare)
    {
        return std::set<Value, Compare>(values, compare);
    }

    //!     Создание множества с явным заданием отношения порядка и аллокатора
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename Value, typename Compare, typename Allocator>
    auto make_set (std::initializer_list<Value> values, const Compare & compare, const Allocator & allocator)
    {
        return std::set<Value, Compare, Allocator>(values, compare, allocator);
    }
} // namespace burst

#endif // BURST_CONTAINER_MAKE_SET_HPP
