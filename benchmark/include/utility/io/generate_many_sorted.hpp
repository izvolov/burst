#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_SORTED_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_SORTED_HPP

#include <utility/io/generate_sorted.hpp>

#include <cstddef>
#include <cstdint>

namespace utility
{
    template
    <
        typename URNG,
        typename BinaryPredicate,
        typename OutputIterator,
        typename RandomAccessIterator
    >
    OutputIterator
        generate_many_sorted
        (
            URNG && generator,
            std::size_t block_size,
            std::size_t range_count,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            BinaryPredicate order,
            OutputIterator result,
            RandomAccessIterator buffer
        )
    {
        for (std::size_t i = 0; i < range_count; ++i)
        {
            result =
                generate_sorted
                (
                    generator,
                    block_size,
                    range_length,
                    min,
                    max,
                    order,
                    result,
                    buffer
                );
        }

        return result;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_MANY_SORTED_HPP
