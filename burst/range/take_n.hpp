#ifndef BURST_RANGE_TAKE_N_HPP
#define BURST_RANGE_TAKE_N_HPP

#include <burst/range/detail/take_n.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/iterator.hpp>

#include <type_traits>

namespace burst
{
    //!     Функция для откусывания нескольких элементов от начала диапазона.
    /*!
            Принимает на вход диапазон и число элементов, которые нужно откусить.
            Возвращает диапазон, состоящий из `n` первых элементов входного диапазона, причём конец
        входного диапазона не отслеживается. Ответственность за случай `n > range.size()` лежит на
        пользователе.
     */
    template <typename Range>
    auto take_n (Range && range, typename boost::range_difference<Range>::type n)
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
} // namespace burst

#endif // BURST_RANGE_TAKE_N_HPP
