#ifndef BURST__CONCEPT__INTEGER_HPP
#define BURST__CONCEPT__INTEGER_HPP

#include <type_traits>

namespace burst
{
    template
    <
        typename T,
        typename =
            std::enable_if_t
            <
                std::is_integral<std::decay_t<T>>::value
            >
    >
    using Integer = T;
} // namespace burst

#endif // BURST__CONCEPT__INTEGER_HPP
