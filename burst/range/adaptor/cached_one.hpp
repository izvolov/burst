#ifndef BURST_RANGE_ADAPTOR_CACHED_ONE_HPP
#define BURST_RANGE_ADAPTOR_CACHED_ONE_HPP

#include <utility>

#include <burst/range/cache_one.hpp>

namespace burst
{
    namespace detail
    {
        struct cache_one_trigger_t {};

        template <typename Range>
        auto operator | (Range && range, cache_one_trigger_t)
        {
            return cache_one(std::forward<Range>(range));
        }
    }

    //!     Инструмент для кэширования элементов диапазона через конвейер
    /*!
            Вызов

                `range | cached_one`

            эквивалентен вызову

                `cache_one(range)`.
     */
    constexpr auto cached_one = detail::cache_one_trigger_t{};
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_CACHED_ONE_HPP
