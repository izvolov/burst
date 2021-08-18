#ifndef BURST__RANGE__DETAIL__SKIP_TO_LOWER_BOUND_HPP
#define BURST__RANGE__DETAIL__SKIP_TO_LOWER_BOUND_HPP

#include <burst/type_traits/iterator_category.hpp>
#include <burst/type_traits/range_iterator.hpp>

#include <boost/range/algorithm/lower_bound.hpp>

#include <iterator>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        //!     Продвижение диапазона произвольного доступа.
        /*!
                Использует двоичный поиск для нахождения в диапазоне места, до которого нужно
            продвинуться, и сразу продвигается до этого места.
         */
        template <typename RandomAccessRange, typename Value, typename Compare>
        typename std::enable_if
        <
            std::is_same
            <
                std::random_access_iterator_tag,
                iterator_category_t<range_iterator_t<RandomAccessRange>>
            >
            ::value,
            void
        >
        ::type
        skip_to_lower_bound (RandomAccessRange & range, const Value & goal, Compare compare)
        {
            range.advance_begin
            (
                std::distance
                (
                    range.begin(),
                    boost::lower_bound(range, goal, compare)
                )
            );
        }

        //!     Продвижение любого диапазона, кроме произвольного доступа.
        /*!
                Поэлементно двигает начало диапазона до тех пор, пока либо диапазон не закончится,
            либо его начало не встанет на то место, до которого нужно продвинуться.
         */
        template <typename InputRange, typename Value, typename Compare>
        typename std::enable_if
        <
            not std::is_same
            <
                std::random_access_iterator_tag,
                iterator_category_t<range_iterator_t<InputRange>>
            >
            ::value,
            void
        >
        ::type
        skip_to_lower_bound (InputRange & range, const Value & goal, Compare compare)
        {
            while (not range.empty() && compare(range.front(), goal))
            {
                range.advance_begin(1);
            }
        }
    }
}

#endif // BURST__RANGE__DETAIL__SKIP_TO_LOWER_BOUND_HPP
