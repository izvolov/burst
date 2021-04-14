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
    template <typename URNG>
    std::ostream &
        generate_one
        (
            URNG && generator,
            std::ostream & stream,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max
        )
    {
        std::uniform_int_distribution<std::int64_t> uniform(min, max);


        burst::trivial_write(stream, static_cast<std::size_t>(range_length));
        for (auto i = 0ul; i < range_length; ++i)
        {
            burst::trivial_write(stream, uniform(generator));
        }

        return stream;
    }

    template <typename URNG>
    std::ostream &
        generate_one_sorted
        (
            URNG && generator,
            std::ostream & stream,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool descending
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

        for (const auto & x: range)
        {
            burst::trivial_write(stream, x);
        }

        return stream;
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

    template <typename URNG>
    std::ostream &
        generate_one_blockwise
        (
            URNG && generator,
            std::ostream & stream,
            std::size_t block_size,
            std::size_t range_length,
            std::int64_t global_min,
            std::int64_t global_max,
            bool descending
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

        burst::trivial_write(stream, range_length);
        for (auto i = 1ul; i < bounds.size() - 1; ++i)
        {
            const auto min = std::min(bounds[i - 1], bounds[i]);
            const auto max = std::max(bounds[i - 1], bounds[i]);
            generate_one_sorted(generator, stream, block_size - 1, min, max, descending);
            burst::trivial_write(stream, bounds[i]);
        }
        const auto min = std::min(bounds[bounds.size() - 2], bounds[bounds.size() - 1]);
        const auto max = std::max(bounds[bounds.size() - 2], bounds[bounds.size() - 1]);
        const auto remainder = range_length % block_size;
        const auto tail = remainder != 0 ? remainder : block_size;
        return generate_one_sorted(generator, stream, tail, min, max, descending);
    }

    template <typename URNG>
    std::ostream &
        generate
        (
            URNG && generator,
            std::ostream & stream,
            std::size_t block_size,
            std::size_t range_count,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool sort,
            bool descending
        )
    {
        for (std::size_t i = 0; i < range_count; ++i)
        {
            if (sort)
            {
                generate_one_blockwise(generator, stream, block_size, range_length, min, max, descending);
            }
            else
            {
                generate_one(generator, stream, range_length, min, max);
            }
        }

        return stream;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
