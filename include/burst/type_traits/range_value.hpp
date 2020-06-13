#ifndef BURST_TYPE_TRAITS_RANGE_VALUE_HPP
#define BURST_TYPE_TRAITS_RANGE_VALUE_HPP

#include <boost/range/value_type.hpp>

namespace burst
{
    template <typename Range>
    using range_value_t = typename boost::range_value<Range>::type;
} // namespace burst

#endif // BURST_TYPE_TRAITS_RANGE_VALUE_HPP
