#ifndef BURST_EXECUTION_PARALLEL_POLICY_HPP
#define BURST_EXECUTION_PARALLEL_POLICY_HPP

#include <cassert>
#include <cstddef>

namespace burst
{
    struct parallel_policy
    {
        std::size_t thread_count;
    };

    template <typename Integral>
    constexpr auto par (Integral n)
    {
        assert(n >= 0);
        return parallel_policy{static_cast<std::size_t>(n)};
    }
} // namespace burst

#endif // BURST_EXECUTION_PARALLEL_POLICY_HPP
