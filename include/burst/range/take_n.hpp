#ifndef BURST_RANGE_TAKE_N_HPP
#define BURST_RANGE_TAKE_N_HPP

#include <burst/range/take_at_most.hpp>

namespace burst
{
    /*!
        \see take_at_most_t
     */
    constexpr auto take_n = take_at_most_t{};
} // namespace burst

#endif // BURST_RANGE_TAKE_N_HPP
