#ifndef BURST_RANGE_TAKE_EXACTLY_HPP
#define BURST_RANGE_TAKE_EXACTLY_HPP

#include <burst/range/detail/take_exactly.hpp>
#include <burst/type_traits/range_iterator.hpp>

#include <boost/iterator/iterator_categories.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    struct take_exactly_t
    {
        /*!
            \brief
                Функция для откусывания нескольких элементов от начала диапазона

            \param range
                Диапазон, от которого нужно откусить кусок.
            \param n
                Размер куска.

            \returns
                Диапазон, состоящий из `n` первых элементов входного диапазона.

            \warning
                Конец входного диапазона не отслеживается. Ответственность за случай
                `n > range.size()` лежит на пользователе.
         */
        template <typename Range, typename Integer>
        auto operator () (Range && range, Integer n) const
        {
            using range_iterator = range_iterator_t<Range>;
            using category =
                typename std::common_type
                <
                    typename boost::iterators::pure_iterator_traversal<range_iterator>::type,
                    boost::random_access_traversal_tag
                >
                ::type;

            return detail::take_exactly_impl(std::forward<Range>(range), n, category{});
        }
    };

    constexpr auto take_exactly = take_exactly_t{};
} // namespace burst

#endif // BURST_RANGE_TAKE_EXACTLY_HPP
