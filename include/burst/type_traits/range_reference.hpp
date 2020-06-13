#ifndef BURST_TYPE_TRAITS_RANGE_REFERENCE_HPP
#define BURST_TYPE_TRAITS_RANGE_REFERENCE_HPP

#include <boost/range/reference.hpp>

namespace burst
{
    template <typename Range>
    using range_reference_t = typename boost::range_reference<Range>::type;
} // namespace burst

#endif // BURST_TYPE_TRAITS_RANGE_REFERENCE_HPP
