#ifndef BURST_RANGE_PURE_TRAVERSAL_HPP
#define BURST_RANGE_PURE_TRAVERSAL_HPP

#include <boost/range/traversal.hpp>
#include <boost/iterator/iterator_categories.hpp>

namespace burst
{
    template <typename Range>
    struct pure_range_traversal
    {
        using type =
            typename boost::iterators::pure_iterator_traversal
            <
                typename boost::range_iterator<Range>::type
            >
            ::type;
    };
} // namespace burst

#endif // BURST_RANGE_PURE_TRAVERSAL_HPP
