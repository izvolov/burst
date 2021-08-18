#ifndef BURST__CONTAINER__MAKE_SET_HPP
#define BURST__CONTAINER__MAKE_SET_HPP

#include <boost/range/value_type.hpp>

#include <initializer_list>
#include <iterator>
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

    //!     Создать std::set из диапазона с явным указанием типов его значений
    /*!
            Отличается тем, что тип значений множества не выводится из типа значений контейнера, а
        указывается пользователем явно.

            `make_set<std::uint32_t>(range)`
     */
    template <typename Value, typename InputRange>
    auto make_set (InputRange && values)
    {
        using std::begin;
        using std::end;
        return
            std::set<Value>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values))
            );
    }

    //!     Создать std::set из диапазона с явным указанием типов его значений и отношением порядка
    /*!
            Отличается тем, что тип значений множества не выводится из типа значений контейнера, а
        указывается пользователем явно.

            `make_set<std::uint32_t>(range, std::greater<>{})`
     */
    template <typename Value, typename InputRange, typename Compare>
    auto make_set (InputRange && values, const Compare & compare)
    {
        using std::begin;
        using std::end;
        return
            std::set<Value, Compare>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values)),
                compare
            );
    }

    //!     Создать std::set из диапазона с явным указанием типов его значений, отношением порядка и
    //! аллокатором
    /*!
            Отличается тем, что тип значений множества не выводится из типа значений контейнера, а
        указывается пользователем явно.

            `make_set<std::uint32_t>(range, std::greater<>{}, allocator)`
     */
    template <typename Value, typename InputRange, typename Compare, typename Allocator>
    auto make_set (InputRange && values, const Compare & compare, const Allocator & allocator)
    {
        using std::begin;
        using std::end;
        return
            std::set<Value, Compare, Allocator>
            (
                begin(std::forward<InputRange>(values)),
                end(std::forward<InputRange>(values)),
                compare,
                allocator
            );
    }

    //!     Создать std::set из диапазона
    /*!
            Принимает произвольный диапазон, из типа его значений выводит тип элементов, создаёт
        std::set и возвращает его.
     */
    template <typename InputRange>
    auto make_set (InputRange && values)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        return make_set<value_type>(std::forward<InputRange>(values));
    }

    //!     Создать std::set из диапазона с явно заданным отношением порядка
    /*!
            Отличается наличием отношения порядка на элементах множества, передаваемого в качестве
        аргумента функции.
     */
    template <typename InputRange, typename Compare>
    auto make_set (InputRange && values, const Compare & compare)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        return make_set<value_type>(std::forward<InputRange>(values), compare);
    }

    //!     Создать std::set из диапазона с явно заданным отношением порядка и аллокатором
    /*!
            Отличается наличием аллокатора, передаваемого в качестве аргумента функции.
     */
    template <typename InputRange, typename Compare, typename Allocator>
    auto make_set (InputRange && values, const Compare & compare, const Allocator & allocator)
    {
        using value_type = typename boost::range_value<InputRange>::type;
        return make_set<value_type>(std::forward<InputRange>(values), compare, allocator);
    }
} // namespace burst

#endif // BURST__CONTAINER__MAKE_SET_HPP
