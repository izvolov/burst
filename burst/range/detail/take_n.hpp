#ifndef BURST_RANGE_DETAIL_TAKE_N_HPP
#define BURST_RANGE_DETAIL_TAKE_N_HPP

#include <iterator>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/iterator_range.hpp>

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/take_n_iterator.hpp>

namespace burst
{
    namespace detail
    {
        //!     Перегрузка для общего случая.
        /*!
                Возвращает диапазон из двух итераторов типа `take_n_iterator`.
         */
        template <typename Range, typename Category>
        auto
            take_n_impl
            (
                Range && range,
                typename boost::range_difference<Range>::type n,
                Category
            )
        {
            auto begin = burst::make_take_n_iterator(std::begin(std::forward<Range>(range)), n);
            auto end = burst::make_take_n_iterator(begin, iterator::end_tag);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }

        //!     Перегрузка для случая, когда входной диапазон — произвольного доступа.
        /*!
                Возвращает диапазон из тех же итераторов, которые были во входном диапазоне.
         */
        template <typename Range>
        auto
            take_n_impl
            (
                Range && range,
                typename boost::range_difference<Range>::type n,
                boost::random_access_traversal_tag
            )
        {
            return boost::make_iterator_range(std::begin(range), std::begin(range) + n);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_RANGE_DETAIL_TAKE_N_HPP
