#ifndef BURST_RANGE_DETAIL_SKIP_TO_UPPER_BOUND_HPP
#define BURST_RANGE_DETAIL_SKIP_TO_UPPER_BOUND_HPP

#include <burst/type_traits/range_iterator.hpp>

#include <boost/range/algorithm/upper_bound.hpp>
#include <boost/range/iterator_range.hpp>

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
                typename std::iterator_traits<range_iterator_t<RandomAccessRange>>::iterator_category
            >
            ::value,
            void
        >
        ::type
        skip_to_upper_bound (RandomAccessRange & range, const Value & goal, Compare compare)
        {
            range.advance_begin
            (
                std::distance
                (
                    range.begin(),
                    boost::upper_bound(range, goal, compare)
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
                typename std::iterator_traits<range_iterator_t<InputRange>>::iterator_category
            >
            ::value,
            void
        >
        ::type
        skip_to_upper_bound (InputRange & range, const Value & goal, Compare compare)
        {
            while (not range.empty() && not compare(goal, range.front()))
            {
                range.advance_begin(1);
            }
        }
    }
}

#endif // BURST_RANGE_DETAIL_SKIP_TO_UPPER_BOUND_HPP
