#ifndef BURST__RANGE__TAKE_AT_MOST_HPP
#define BURST__RANGE__TAKE_AT_MOST_HPP

#include <burst/range/detail/take_at_most.hpp>
#include <burst/type_traits/iterator_category.hpp>
#include <burst/type_traits/minimum_category.hpp>

#include <iterator>
#include <utility>

namespace burst
{
    struct take_at_most_t
    {
        /*!
            \brief
                Функция для откусывания не более `n` элементов от диапазона

            \param range
                Диапазон, от которого нужно откусить кусок.
            \param n
                Размер куска.

            \returns
                Диапазон, состоящий из `n` первых элементов входного диапазона, если в исходном
                диапазоне не менее `n` элементов, либо диапазон, состоящий из всех элементов
                исходного диапазона, если в исходном диапазоне менее `n` элементов.

            \see take_at_most_iterator
            \see make_take_at_most_iterator
         */
        template <typename Iterator, typename Integer>
        auto operator () (Iterator first, Iterator last, Integer n) const
        {
            using category =
                minimum_category_t
                <
                    iterator_category_t<Iterator>,
                    std::random_access_iterator_tag
                >;

            return detail::take_at_most_impl(first, last, n, category{});
        }

        /*!
            \brief
                Функция для откусывания не более `n` элементов от диапазона

            \param r
                Диапазон, от которого нужно откусить кусок.
            \param n
                Размер куска.

            \returns
                Диапазон, состоящий из `n` первых элементов входного диапазона, если в исходном
                диапазоне не менее `n` элементов, либо диапазон, состоящий из всех элементов
                исходного диапазона, если в исходном диапазоне менее `n` элементов.

            \see take_at_most_iterator
            \see make_take_at_most_iterator
         */
        template <typename Range, typename Integer>
        auto operator () (Range && r, Integer n) const
        {
            using std::begin;
            using std::end;
            return this->operator()(begin(std::forward<Range>(r)), end(std::forward<Range>(r)), n);
        }
    };

    constexpr auto take_at_most = take_at_most_t{};
} // namespace burst

#endif // BURST__RANGE__TAKE_AT_MOST_HPP
