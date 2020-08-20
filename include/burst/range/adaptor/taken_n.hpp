#ifndef BURST_RANGE_ADAPTOR_TAKEN_N_HPP
#define BURST_RANGE_ADAPTOR_TAKEN_N_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/take_n.hpp>

namespace burst
{
    /*!
        \see take_n
        \see taken_at_most
     */
    constexpr auto taken_n = make_adaptor_trigger(take_n);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TAKEN_N_HPP
