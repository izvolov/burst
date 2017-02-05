#ifndef BURST_RANGE_TAKE_N_HPP
#define BURST_RANGE_TAKE_N_HPP

#include <burst/range/detail/take_n.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/iterator.hpp>

#include <type_traits>

namespace burst
{
    struct take_n_t
    {
        //!     Функция для откусывания нескольких элементов от начала диапазона.
        /*!
                Принимает на вход диапазон и число элементов, которые нужно откусить.
                Возвращает диапазон, состоящий из `n` первых элементов входного диапазона, причём конец
            входного диапазона не отслеживается. Ответственность за случай `n > range.size()` лежит на
            пользователе.
         */
        template <typename Range, typename Integer>
        auto operator () (Range && range, Integer n) const
        {
            using range_iterator = typename boost::range_iterator<Range>::type;
            using category =
                typename std::common_type
                <
                    typename boost::iterators::pure_iterator_traversal<range_iterator>::type,
                    boost::random_access_traversal_tag
                >
                ::type;

            return detail::take_n_impl(std::forward<Range>(range), n, category{});
        }
    };

    constexpr auto take_n = take_n_t{};
} // namespace burst

#endif // BURST_RANGE_TAKE_N_HPP
