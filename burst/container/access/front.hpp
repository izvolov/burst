#ifndef BURST_CONTAINER_ACCESS_FRONT_HPP
#define BURST_CONTAINER_ACCESS_FRONT_HPP

namespace burst
{
    template <class T, std::size_t Size>
    inline decltype(auto) front (T (& array) [Size])
    {
        return array[0];
    }

    template <class Container>
    inline decltype(auto) front (Container & container)
    {
        return container.front();
    }
} // namespace burst

#endif // BURST_CONTAINER_ACCESS_FRONT_HPP
