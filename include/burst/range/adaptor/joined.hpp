#ifndef BURST__RANGE__ADAPTOR__JOINED_HPP
#define BURST__RANGE__ADAPTOR__JOINED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/join.hpp>

namespace burst
{
    //!     Инструмент для склеивания диапазонов через конвейер.
    /*!
            Вызов

                `ranges | joined`

            эквивалентен вызову

                `join(ranges)`.
     */
    constexpr auto joined = make_adaptor_trigger(join);
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__JOINED_HPP
