#ifndef BURST_CONTAINER_ACCESS_CFRONT_HPP
#define BURST_CONTAINER_ACCESS_CFRONT_HPP

namespace burst
{
    template <class T, std::size_t Size>
    inline decltype(auto) cfront (const T (& array) [Size])
    {
        return array[0];
    }

    template <class Container>
    inline decltype(auto) cfront (const Container & container)
    {
        return container.front();
    }
} // namespace burst

#endif // BURST_CONTAINER_ACCESS_CFRONT_HPP
