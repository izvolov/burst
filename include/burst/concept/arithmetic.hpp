#ifndef BURST__CONCEPT__ARITHMETIC_HPP
#define BURST__CONCEPT__ARITHMETIC_HPP

#include <type_traits>

namespace burst
{
    template
    <
        typename T,
        typename =
            std::enable_if_t
            <
                std::is_arithmetic<std::decay_t<T>>::value
            >
    >
    using Arithmetic = T;
} // namespace burst

#endif // BURST__CONCEPT__ARITHMETIC_HPP
