#ifndef BURST__RANGE__DETAIL__TAKE_AT_MOST_HPP
#define BURST__RANGE__DETAIL__TAKE_AT_MOST_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/take_at_most_iterator.hpp>
#include <burst/type_traits/iterator_difference.hpp>

#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <iterator>
#include <utility>

namespace burst
{
    namespace detail
    {
        /*!
            \brief
                Перегрузка для общего случая

            \returns
                Диапазон из двух итераторов типа `take_at_most_iterator`.
         */
        template <typename Iterator, typename Integer, typename Category>
        auto take_at_most_impl (Iterator first, Iterator last, Integer n, Category)
        {
            auto begin = burst::make_take_at_most_iterator(first, last, n);
            auto end = burst::make_take_at_most_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        /*!
            \brief
                Перегрузка для случая, когда входной диапазон — произвольного доступа

            \returns
                Диапазон из тех же итераторов, которые были во входном диапазоне.
         */
        template <typename RandomAccessIterator, typename Integer>
        auto
            take_at_most_impl
            (
                RandomAccessIterator first,
                RandomAccessIterator last,
                Integer n,
                std::random_access_iterator_tag
            )
        {
            using difference_type = iterator_difference_t<RandomAccessIterator>;
            auto m = std::min(static_cast<difference_type>(n), std::distance(first, last));
            return boost::make_iterator_range(first, std::next(first, m));
        }
    } // namespace detail
} // namespace burst

#endif // BURST__RANGE__DETAIL__TAKE_AT_MOST_HPP
