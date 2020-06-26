#ifndef BURST_BENCHMARK_UTILITY_IO_READ_HPP
#define BURST_BENCHMARK_UTILITY_IO_READ_HPP

#include <burst/functional/trivial_read.hpp>

#include <cstddef>
#include <cstdint>
#include <istream>

template <typename Container>
std::istream & read (std::istream & stream, Container & values)
{
    auto size = std::size_t{};
    if (burst::trivial_read(stream, size))
    {
        values.resize(size);

        for (auto & value: values)
        {
            auto n = std::int64_t{};
            burst::trivial_read(stream, n);
            value = static_cast<typename Container::value_type>(n);
        }
    }

    return stream;
}

#endif // BURST_BENCHMARK_UTILITY_IO_READ_HPP
