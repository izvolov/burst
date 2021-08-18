#ifndef BURST__TYPE_TRAITS__RANGE_VALUE_HPP
#define BURST__TYPE_TRAITS__RANGE_VALUE_HPP

#include <boost/range/value_type.hpp>

namespace burst
{
    template <typename Range>
    using range_value_t = typename boost::range_value<Range>::type;
} // namespace burst

#endif // BURST__TYPE_TRAITS__RANGE_VALUE_HPP
