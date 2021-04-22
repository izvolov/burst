#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_SORTED_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_SORTED_HPP

#include <utility/io/detail/generate_blockwise.hpp>

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
        generate_sorted
        (
            URNG && generator,
            std::size_t block_size,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            BinaryPredicate order,
            OutputIterator result,
            RandomAccessIterator buffer
        )
    {
        return
            detail::generate_blockwise
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
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_SORTED_HPP
