#ifndef BURST_RANGE_ADAPTOR_TAKEN_N_HPP
#define BURST_RANGE_ADAPTOR_TAKEN_N_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/take_n.hpp>

namespace burst
{
    //!     Инструмент для откусывания диапазона через конвейер.
    /*!
            Вызов

                `range | taken_n(10)`

            эквивалентен вызову

                `take_n(range, 10)`.
     */
    constexpr auto taken_n = make_adaptor_trigger(take_n);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TAKEN_N_HPP
