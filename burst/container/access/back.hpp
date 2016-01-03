#ifndef BURST_CONTAINER_ACCESS_BACK_HPP
#define BURST_CONTAINER_ACCESS_BACK_HPP

#include <cstddef>

namespace burst
{
    template <class T, std::size_t Size>
    inline decltype(auto) back (T (& array) [Size])
    {
        return array[Size - 1];
    }

    template <class Container>
    inline decltype(auto) back (Container & container)
    {
        return container.back();
    }
} // namespace burst

#endif // BURST_CONTAINER_ACCESS_BACK_HPP
