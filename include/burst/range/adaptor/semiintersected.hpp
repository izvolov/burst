#ifndef BURST__RANGE__ADAPTOR__SEMIINTERSECTED_HPP
#define BURST__RANGE__ADAPTOR__SEMIINTERSECTED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/semiintersect.hpp>

namespace burst
{
    //!     Инструмент для полупересечения диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | semiintersected`
                `range | semiintersected(std::greater<>{})`

            соответственно эквивалентен вызовам

                `semiintersect(range)`
                `semiintersect(range, std::greater<>{})`.
     */
    constexpr auto semiintersected = make_adaptor_trigger(semiintersect);
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__SEMIINTERSECTED_HPP
