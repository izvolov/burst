#ifndef BURST_TYPE_TRAITS_RANGE_ITERATOR_HPP
#define BURST_TYPE_TRAITS_RANGE_ITERATOR_HPP

#include <boost/range/iterator.hpp>

namespace burst
{
    template <typename Range>
    using range_iterator_t = typename boost::range_iterator<Range>::type;
} // namespace burst

#endif // BURST_TYPE_TRAITS_RANGE_ITERATOR_HPP
