#ifndef BURST__RANGE__ADAPTOR__OWNED_AS_RANGE_HPP
#define BURST__RANGE__ADAPTOR__OWNED_AS_RANGE_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/own_as_range.hpp>

namespace burst
{
    //!     Инструмент для принятия владения контейнером
    /*!
            Вызов

                `range | owned_as_range`

            эквивалентен вызову

                `own_as_range(range)`.
     */
    constexpr auto owned_as_range = make_adaptor_trigger(own_as_range);
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__OWNED_AS_RANGE_HPP
