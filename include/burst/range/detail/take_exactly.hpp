#ifndef BURST_RANGE_DETAIL_TAKE_EXACTLY_HPP
#define BURST_RANGE_DETAIL_TAKE_EXACTLY_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/take_exactly_iterator.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/iterator_range.hpp>

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
                Диапазон из двух итераторов типа `take_exactly_iterator`.
         */
        template <typename Range, typename Integer, typename Category>
        auto take_exactly_impl (Range && range, Integer n, Category)
        {
            using std::begin;
            auto first = burst::make_take_exactly_iterator(begin(std::forward<Range>(range)), n);
            auto last = burst::make_take_exactly_iterator(iterator::end_tag, first);

            return boost::make_iterator_range(std::move(first), std::move(last));
        }

        /*!
            \brief
                Перегрузка для случая, когда входной диапазон — произвольного доступа

            \returns
                Диапазон из тех же итераторов, которые были во входном диапазоне.
         */
        template <typename Range, typename Integer>
        auto take_exactly_impl (Range && range, Integer n, boost::random_access_traversal_tag)
        {
            using std::begin;
            return boost::make_iterator_range(begin(range), begin(range) + n);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_RANGE_DETAIL_TAKE_EXACTLY_HPP
