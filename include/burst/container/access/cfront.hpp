#ifndef BURST__CONTAINER__ACCESS__CFRONT_HPP
#define BURST__CONTAINER__ACCESS__CFRONT_HPP

#include <cstddef>

namespace burst
{
    struct cfront_t
    {
        template <class T, std::size_t Size>
        constexpr decltype(auto) operator () (const T (& array) [Size]) const
        {
            return array[0];
        }

        template <class Container>
        constexpr decltype(auto) operator () (const Container & container) const
        {
            return container.front();
        }
    };

    constexpr auto cfront = cfront_t{};
} // namespace burst

#endif // BURST__CONTAINER__ACCESS__CFRONT_HPP
