#ifndef BURST_RANGE_ADAPTOR_SYMMETRIC_DIFFERENCED_HPP
#define BURST_RANGE_ADAPTOR_SYMMETRIC_DIFFERENCED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/symmetric_difference.hpp>

namespace burst
{
    //!     Инструмент для нахождения симметрической разности диапазона диапазонов через конвейер
    /*!
            Вызовы

                `range | symmetric_differenced`
                `range | symmetric_differenced(std::greater<>{})`

            соответственно эквивалентны вызовам

                `symmetric_difference(range)`
                `symmetric_difference(range, std::greater<>{})`.
     */
    constexpr auto symmetric_differenced = make_adaptor_trigger(symmetric_difference);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_SYMMETRIC_DIFFERENCED_HPP
