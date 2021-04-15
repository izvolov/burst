#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>

namespace utility
{
    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate
        (
            URNG && generator,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            OutputIterator result
        )
    {
        std::uniform_int_distribution<std::int64_t> uniform(min, max);

        *result++ = range_length;
        return
            std::generate_n(result, range_length,
                [& uniform, & generator] {return uniform(generator);});
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
