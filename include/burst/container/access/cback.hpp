#ifndef BURST__CONTAINER__ACCESS__CBACK_HPP
#define BURST__CONTAINER__ACCESS__CBACK_HPP

#include <cstddef>

namespace burst
{
    struct cback_t
    {
        template <class T, std::size_t Size>
        constexpr decltype(auto) operator () (const T (& array) [Size]) const
        {
            return array[Size - 1];
        }

        template <class Container>
        constexpr decltype(auto) operator () (const Container & container) const
        {
            return container.back();
        }
    };

    constexpr auto cback = cback_t{};
} // namespace burst

#endif // BURST__CONTAINER__ACCESS__CBACK_HPP
