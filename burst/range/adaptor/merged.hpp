#ifndef BURST_RANGE_ADAPTOR_MERGED_HPP
#define BURST_RANGE_ADAPTOR_MERGED_HPP

#include <burst/range/merge.hpp>

#include <utility>

namespace burst
{
    namespace detail
    {
        template <typename Compare>
        struct merge_forwarder_t
        {
            Compare value;
        };

        struct merge_trigger_t
        {
            template <typename Compare>
            auto operator () (Compare compare) const
            {
                return merge_forwarder_t<Compare>{std::move(compare)};
            }
        };

        template <typename Range, typename Compare>
        auto operator | (Range && range, merge_forwarder_t<Compare> compare)
        {
            return merge(std::forward<Range>(range), std::move(compare.value));
        }

        template <typename Range>
        auto operator | (Range && range, const merge_trigger_t &)
        {
            return merge(std::forward<Range>(range));
        }
    }

    //!     Инструмент для слияния диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | merged`
                `range | merged(std::greater<>{})`

            соответственно эквивалентен вызовам

                `merge(range)`
                `merge(range, std::greater<>{})`.
     */
    constexpr auto merged = detail::merge_trigger_t{};
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_MERGED_HPP
