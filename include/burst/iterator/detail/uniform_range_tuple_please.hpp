#ifndef BURST__ITERATOR__DETAIL__UNIFORM_RANGE_TUPLE_PLEASE_HPP
#define BURST__ITERATOR__DETAIL__UNIFORM_RANGE_TUPLE_PLEASE_HPP

#include <burst/range/to_any_range.hpp>
#include <burst/tuple/by_all.hpp>
#include <burst/type_traits/are_same.hpp>
#include <burst/type_traits/minimum_category.hpp>
#include <burst/type_traits/range_category.hpp>

#include <boost/iterator/iterator_categories.hpp>

#include <tuple>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        /*!
            \brief
                Перегрузка для случая, когда все переданные диапазоны имеют один и тот же тип

            \see uniform_range_tuple_please
         */
        template <typename... Ranges>
        auto uniform_range_tuple_please_impl (std::tuple<Ranges &...> ranges, std::true_type)
        {
            static_assert(are_same_v<Ranges...>, "");
            return ranges;
        }

        /*!
            \brief
                Перегрузка для случая, когда переданные диапазоны разнотипны

            \see uniform_range_tuple_please
         */
        template <typename... Ranges>
        auto uniform_range_tuple_please_impl (std::tuple<Ranges &...> ranges, std::false_type)
        {
            static_assert(not are_same_v<Ranges...>, "");
            using iterator_category = minimum_category_t<range_category_t<Ranges>...>;
            using boost_traversal =
                typename boost::iterator_category_to_traversal<iterator_category>::type;
            return by_all(to_any_range<boost_traversal>, ranges);
        }

        /*!
            \brief
                Сделать из кортежа возможно разнотипных диапазонов кортеж заведомо однотипных
                диапазонов

            \returns
                Если диапазоны изначально однотипны, то возвращается изначальный кортеж, в
                противном случае создаётся кортеж, в котором каждый из диапазонов приведён к
                boost::any_range.

            \see are_same
         */
        template <typename... Ranges>
        auto uniform_range_tuple_please (std::tuple<Ranges &...> ranges)
        {
            return uniform_range_tuple_please_impl(ranges, are_same<Ranges...>{});
        }
    }
}

#endif // BURST__ITERATOR__DETAIL__UNIFORM_RANGE_TUPLE_PLEASE_HPP
