#ifndef BURST_RANGE_ADAPTOR_TAKEN_AT_MOST_HPP
#define BURST_RANGE_ADAPTOR_TAKEN_AT_MOST_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/take_at_most.hpp>

namespace burst
{
    /*!
        \brief
            Инструмент для откусывания диапазона через конвейер.

        \details
            Вызов

            \code{.cpp}
            range | taken_at_most(10)
            \endcode

            эквивалентен вызову

            \code{.cpp}
            take_at_most(range, 10)
            \endcode
     */
    constexpr auto taken_at_most = make_adaptor_trigger(take_at_most);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TAKEN_AT_MOST_HPP
