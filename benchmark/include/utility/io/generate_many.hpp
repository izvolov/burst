#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_HPP

#include <utility/io/generate.hpp>

#include <cstddef>
#include <cstdint>

namespace utility
{
    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate_many
        (
            URNG && generator,
            std::size_t range_count,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            OutputIterator result
        )
    {
        for (std::size_t i = 0; i < range_count; ++i)
        {
            result = generate(generator, range_length, min, max, result);
        }

        return result;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_HPP
