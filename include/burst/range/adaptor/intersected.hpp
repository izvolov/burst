#ifndef BURST_RANGE_ADAPTOR_INTERSECTED_HPP
#define BURST_RANGE_ADAPTOR_INTERSECTED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/intersect.hpp>

namespace burst
{
    //!     Инструмент для пересечения диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | intersected`
                `range | intersected(std::greater<>{})`

            соответственно эквивалентен вызовам

                `intersect(range)`
                `intersect(range, std::greater<>{})`.
     */
    constexpr auto intersected = make_adaptor_trigger(intersect);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_INTERSECTED_HPP
