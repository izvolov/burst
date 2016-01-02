#ifndef BURST_RANGE_ADAPTOR_JOINED_HPP
#define BURST_RANGE_ADAPTOR_JOINED_HPP

#include <utility>

#include <burst/range/join.hpp>

namespace burst
{
    namespace detail
    {
        struct join_trigger_t {};

        template <typename Range>
        auto operator | (Range && range, join_trigger_t)
        {
            return join(std::forward<Range>(range));
        }
    }

    //!     Инструмент для склеивания диапазонов через конвейер.
    /*!
            Вызов

                `ranges | joined`

            эквивалентен вызову

                `join(ranges)`.
     */
    constexpr auto joined = detail::join_trigger_t{};
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_JOINED_HPP
