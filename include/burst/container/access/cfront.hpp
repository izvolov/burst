#ifndef BURST_CONTAINER_ACCESS_CFRONT_HPP
#define BURST_CONTAINER_ACCESS_CFRONT_HPP

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

#endif // BURST_CONTAINER_ACCESS_CFRONT_HPP
