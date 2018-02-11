#ifndef BURST_CONCEPT_ARITHMETIC_HPP
#define BURST_CONCEPT_ARITHMETIC_HPP

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

#endif // BURST_CONCEPT_ARITHMETIC_HPP
