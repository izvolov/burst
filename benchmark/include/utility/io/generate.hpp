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
    template <typename URNG>
    std::ostream &
        generate_one
        (
            URNG && generator,
            std::ostream & stream,
            std::size_t range_length,
            std::int64_t min,
            std::int64_t max,
            bool sort,
            bool descending
        )
    {
        std::uniform_int_distribution<std::int64_t> uniform(min, max);
        std::vector<std::int64_t> range(range_length);

        std::generate(range.begin(), range.end(),
            [& uniform, & generator] {return uniform(generator);});

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

        return write(stream, range);
    }

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
        std::default_random_engine generator(seed_value);

        for (std::size_t i = 0; i < range_count; ++i)
        {
            generate_one(generator, stream, range_length, min, max, sort, descending);
        }

        return stream;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_GENERATE_HPP
