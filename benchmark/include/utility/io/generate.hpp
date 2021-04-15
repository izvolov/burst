#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP

#include <burst/functional/trivial_write.hpp>
#include <random/urd_order_statistic_distribution.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <random>
#include <vector>

namespace utility
{
    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate_one
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
        for (auto i = 0ul; i < range_length; ++i)
        {
            *result++ = uniform(generator);
        }

        return result;
    }

    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate_sorted_part
        (
            URNG && generator,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool descending,
            OutputIterator result
        )
    {
        std::uniform_int_distribution<std::int64_t> uniform(min, max);
        std::vector<std::int64_t> range(range_length);

        std::generate(range.begin(), range.end(),
            [& uniform, & generator] {return uniform(generator);});

        if (descending)
        {
            std::sort(range.begin(), range.end(), std::greater<>{});
        }
        else
        {
            std::sort(range.begin(), range.end());
        }

        return std::copy(range.begin(), range.end(), result);
    }

    template <typename URNG>
    auto
        generate_block_bounds
        (
            URNG && generator,
            std::size_t block_size,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max
        )
    {
        const auto n = range_length;
        auto bounds = std::vector<std::int64_t>{min};

        auto current_min = min;

        const auto remainder = range_length % block_size;
        const auto block_count = range_length / block_size + static_cast<std::size_t>(remainder != 0);
        for (auto i = 0ul; i < block_count - 1; ++i)
        {
            const auto k = (i + 1) * block_size;

            auto osd = utility::urd_order_statistic_distribution<double>(n, k, min, max);
            const auto kth_stat = std::llround(osd(generator));

            std::clog << k << " th: " << kth_stat << std::endl;

            bounds.push_back(kth_stat);

            current_min = kth_stat;
        }
        bounds.push_back(max);

        return bounds;
    }

    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate_one_blockwise
        (
            URNG && generator,
            std::size_t block_size,
            std::size_t range_length,
            std::int64_t global_min,
            std::int64_t global_max,
            bool descending,
            OutputIterator result
        )
    {
        auto bounds =
            generate_block_bounds(generator, block_size, range_length, global_min, global_max);
        if (descending)
        {
            std::sort(bounds.begin(), bounds.end(), std::greater<>{});
        }
        else
        {
            std::sort(bounds.begin(), bounds.end());
        }

        *result++ = range_length;
        for (auto i = 1ul; i < bounds.size() - 1; ++i)
        {
            const auto min = std::min(bounds[i - 1], bounds[i]);
            const auto max = std::max(bounds[i - 1], bounds[i]);
            result = generate_sorted_part(generator, block_size - 1, min, max, descending, result);
            *result++ = bounds[i];
        }
        const auto min = std::min(bounds[bounds.size() - 2], bounds[bounds.size() - 1]);
        const auto max = std::max(bounds[bounds.size() - 2], bounds[bounds.size() - 1]);
        const auto remainder = range_length % block_size;
        const auto tail = remainder != 0 ? remainder : block_size;
        return generate_sorted_part(generator, tail, min, max, descending, result);
    }

    template <typename URNG, typename OutputIterator>
    OutputIterator
        generate
        (
            URNG && generator,
            std::size_t block_size,
            std::size_t range_count,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool sort,
            bool descending,
            OutputIterator result
        )
    {
        for (std::size_t i = 0; i < range_count; ++i)
        {
            if (sort)
            {
                result =
                    generate_one_blockwise(generator, block_size, range_length, min, max, descending, result);
            }
            else
            {
                result = generate_one(generator, range_length, min, max, result);
            }
        }

        return result;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
