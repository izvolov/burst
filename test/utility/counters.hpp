#ifndef BURST_TEST_UTILITY_COUNTERS_HPP
#define BURST_TEST_UTILITY_COUNTERS_HPP

#include <cstddef>

namespace utility
{
    struct copy_move_constructor_counter
    {
        explicit copy_move_constructor_counter (std::size_t & copy_count, std::size_t & move_count):
            copy_count(copy_count),
            move_count(move_count)
        {
        }

        copy_move_constructor_counter (const copy_move_constructor_counter & that):
            copy_count(that.copy_count),
            move_count(that.move_count)
        {
            ++copy_count;
        }

        copy_move_constructor_counter (copy_move_constructor_counter && that):
            copy_count(that.copy_count),
            move_count(that.move_count)
        {
            ++move_count;
        }

        std::size_t & copy_count;
        std::size_t & move_count;
    };
}

#endif // BURST_TEST_UTILITY_COUNTERS_HPP
