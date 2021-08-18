#ifndef BURST__ALGORITHM__DETAIL__COPY_AT_MOST_N_HPP
#define BURST__ALGORITHM__DETAIL__COPY_AT_MOST_N_HPP

#include <burst/type_traits/iterator_difference.hpp>

#include <algorithm>
#include <iterator>
#include <tuple>

namespace burst
{
    namespace detail
    {
        /*!
            \brief
                Общий случай
         */
        template <typename InputIterator, typename Integral, typename OutputIterator>
        std::tuple<InputIterator, Integral, OutputIterator>
            copy_at_most_n_impl
            (
                InputIterator first,
                InputIterator last,
                Integral n,
                OutputIterator result,
                std::input_iterator_tag
            )
        {
            auto m = n;
            while (first != last && m > 0)
            {
                *result = *first;

                ++first;
                ++result;
                --m;
            }

            return std::make_tuple(first, n - m, result);
        }

        /*!
            \brief
                Случай итератора произвольного доступа
         */
        template <typename RandomAccessIterator, typename Integral, typename OutputIterator>
        std::tuple<RandomAccessIterator, Integral, OutputIterator>
            copy_at_most_n_impl
            (
                RandomAccessIterator first,
                RandomAccessIterator last,
                Integral n,
                OutputIterator result,
                std::random_access_iterator_tag
            )
        {
            using difference_type = iterator_difference_t<RandomAccessIterator>;
            const auto size = std::min(static_cast<difference_type>(n), std::distance(first, last));
            return std::make_tuple(std::next(first, size), size, std::copy_n(first, size, result));
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__COPY_AT_MOST_N_HPP
