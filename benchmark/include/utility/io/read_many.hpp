#ifndef BURST_BENCHMARK_UTILITY_IO_READ_MANY_HPP
#define BURST_BENCHMARK_UTILITY_IO_READ_MANY_HPP

#include <utility/io/read.hpp>

#include <istream>

namespace utility
{
    template <typename Container>
    std::istream & read_many (std::istream & stream, Container & containers)
    {
        auto container = typename Container::value_type{};
        while (read(stream, container))
        {
            containers.push_back(container);
        }

        return stream;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_READ_MANY_HPP
