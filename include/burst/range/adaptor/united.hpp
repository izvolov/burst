#ifndef BURST_RANGE_ADAPTOR_UNITED_HPP
#define BURST_RANGE_ADAPTOR_UNITED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/unite.hpp>

namespace burst
{
    //!     Инструмент для объединения диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | united`
                `range | united(std::greater<>{})`

            соответственно эквивалентен вызовам

                `unite(range)`
                `unite(range, std::greater<>{})`.
     */
    constexpr auto united = make_adaptor_trigger(unite);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_UNITED_HPP
