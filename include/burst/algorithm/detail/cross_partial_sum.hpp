#ifndef BURST__ALGORITHM__DETAIL__CROSS_PARTIAL_SUM_HPP
#define BURST__ALGORITHM__DETAIL__CROSS_PARTIAL_SUM_HPP

#include <cstddef>

namespace burst
{
    namespace detail
    {
        // TODO: https://github.com/izvolov/burst/issues/149
        template <typename RandomAccessIterator>
        void
            cross_partial_sum
            (
                RandomAccessIterator values,
                std::size_t block_count,
                std::size_t block_size
            )
        {
            for (auto global_pos = 1ul; global_pos < block_count * block_size; ++global_pos)
            {
                auto prev_block = (global_pos - 1) % block_count;
                auto prev_block_pos = (global_pos - 1) / block_count;

                auto current_block = global_pos % block_count;
                auto current_block_pos = global_pos / block_count;

                values[current_block][current_block_pos] += values[prev_block][prev_block_pos];
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__CROSS_PARTIAL_SUM_HPP
