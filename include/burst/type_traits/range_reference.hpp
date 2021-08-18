#ifndef BURST__TYPE_TRAITS__RANGE_REFERENCE_HPP
#define BURST__TYPE_TRAITS__RANGE_REFERENCE_HPP

#include <boost/range/reference.hpp>

namespace burst
{
    template <typename Range>
    using range_reference_t = typename boost::range_reference<Range>::type;
} // namespace burst

#endif // BURST__TYPE_TRAITS__RANGE_REFERENCE_HPP
