#ifndef BURST_CONTAINER_ACCESS_CBACK_HPP
#define BURST_CONTAINER_ACCESS_CBACK_HPP

#include <cstddef>

namespace burst
{
    template <class T, std::size_t Size>
    inline decltype(auto) cback (const T (& array) [Size])
    {
        return array[Size - 1];
    }

    template <class Container>
    inline decltype(auto) cback (const Container & container)
    {
        return container.back();
    }
} // namespace burst

#endif // BURST_CONTAINER_ACCESS_CBACK_HPP
