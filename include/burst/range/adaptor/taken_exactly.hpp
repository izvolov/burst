#ifndef BURST_RANGE_ADAPTOR_TAKEN_EXACTLY_HPP
#define BURST_RANGE_ADAPTOR_TAKEN_EXACTLY_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/take_exactly.hpp>

namespace burst
{
    /*!
        \brief
            Инструмент для откусывания диапазона через конвейер.

        \details
            Вызов

            \code{.cpp}
            range | taken_exactly(10)
            \endcode

            эквивалентен вызову

            \code{.cpp}
            take_exactly(range, 10)
            \endcode
     */
    constexpr auto taken_exactly = make_adaptor_trigger(take_exactly);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TAKEN_EXACTLY_HPP
