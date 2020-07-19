#ifndef BURST_RANGE_TO_ANY_RANGE_HPP
#define BURST_RANGE_TO_ANY_RANGE_HPP

#include <burst/type_traits/range_reference.hpp>
#include <burst/type_traits/range_value.hpp>

#include <boost/range/any_range.hpp>

namespace burst
{
    /*!
        \brief
            Стирает настоящий тип диапазона

        \details
            Создаёт диапазон, абстрагированный от типа контейнера. Полученный диапазон имеет те же
            самые характеристики, что и исходный диапазон, но имеет некий "универсальный" тип, в
            котором имеет значение только категория итератора, его значение и т.п.

        \tparam Traversal
        \parblock
            Способ обхода итератора. Нечто типа бустового эквивалента категории итератора.
            Его можно получить из обычной std-категории с помощью специальной метафункции:

            \code{.cpp}
            using boost_traversal =
                typename boost::iterator_category_to_traversal<iterator_category>::type;
            \endcode
        \endparblock
    */
    template <typename Traversal>
    struct to_any_range_t
    {
        template <typename Range>
        constexpr auto operator () (Range && range) const
        {
            using value_type = range_value_t<Range>;
            using reference_type = range_reference_t<Range>;
            return boost::any_range<value_type, Traversal, reference_type>(range);
        }
    };

    template <typename Traversal>
    constexpr auto to_any_range = to_any_range_t<Traversal>{};
} // namespace burst

#endif // BURST_RANGE_TO_ANY_RANGE_HPP
