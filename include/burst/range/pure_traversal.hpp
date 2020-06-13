#ifndef BURST_RANGE_PURE_TRAVERSAL_HPP
#define BURST_RANGE_PURE_TRAVERSAL_HPP

#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/traversal.hpp>

namespace burst
{
    template <typename Range>
    struct range_pure_traversal
    {
        using type =
            typename boost::iterators::pure_iterator_traversal
            <
                typename boost::range_iterator<Range>::type
            >
            ::type;
    };

    template <typename Range>
    using range_pure_traversal_t = typename range_pure_traversal<Range>::type;
} // namespace burst

#endif // BURST_RANGE_PURE_TRAVERSAL_HPP
