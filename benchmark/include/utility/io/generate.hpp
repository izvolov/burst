#ifndef BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
#define BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP

#include <utility/io/write.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <random>
#include <vector>

namespace utility
{
    std::ostream &
        generate
        (
            std::ostream & stream,
            std::size_t range_count,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool seed,
            bool sort,
            bool descending
        )
    {
        auto seed_value = seed
            ? static_cast<std::default_random_engine::result_type>
            (
                std::chrono::system_clock::now().time_since_epoch().count()
            )
            : 0;
        std::default_random_engine engine(seed_value);
        std::uniform_int_distribution<std::int64_t> uniform(min, max);

        std::vector<std::int64_t> range(range_length);
        for (std::size_t i = 0; i < range_count; ++i)
        {
            std::generate(range.begin(), range.end(), [&] () { return uniform(engine); });
            if (sort)
            {
                if (descending)
                {
                    std::sort(range.begin(), range.end(), std::greater<>{});
                }
                else
                {
                    std::sort(range.begin(), range.end());
                }
            }

            write(stream, range);
        }

        return stream;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
