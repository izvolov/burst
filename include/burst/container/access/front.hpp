#ifndef BURST_CONTAINER_ACCESS_FRONT_HPP
#define BURST_CONTAINER_ACCESS_FRONT_HPP

#include <cstddef>

namespace burst
{
    struct front_t
    {
        template <class T, std::size_t Size>
        constexpr decltype(auto) operator () (T (& array) [Size]) const
        {
            return array[0];
        }

        template <class Container>
        constexpr decltype(auto) operator () (Container & container) const
        {
            return container.front();
        }
    };

    constexpr auto front = front_t{};
} // namespace burst

#endif // BURST_CONTAINER_ACCESS_FRONT_HPP
