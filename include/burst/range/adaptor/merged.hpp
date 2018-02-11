#ifndef BURST_RANGE_ADAPTOR_MERGED_HPP
#define BURST_RANGE_ADAPTOR_MERGED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/merge.hpp>

namespace burst
{
    //!     Инструмент для слияния диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | merged`
                `range | merged(std::greater<>{})`

            соответственно эквивалентен вызовам

                `merge(range)`
                `merge(range, std::greater<>{})`.
     */
    constexpr auto merged = make_adaptor_trigger(merge);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_MERGED_HPP
