#ifndef BURST_ITERATOR_DETAIL_RANGE_RANGE_HPP
#define BURST_ITERATOR_DETAIL_RANGE_RANGE_HPP

#include <boost/range/reference.hpp>
#include <boost/range/value_type.hpp>

namespace burst
{
    namespace detail
    {
        template <typename Range>
        struct range_range_value
        {
            using type =
                typename boost::range_value
                <
                    typename boost::range_value<Range>::type
                >
                ::type;
        };

        template <typename Range>
        struct range_range_reference
        {
            using type =
                typename boost::range_reference
                <
                    typename boost::range_value<Range>::type
                >
                ::type;
        };

        template <typename Range>
        using range_range_value_t = typename range_range_value<Range>::type;

        template <typename Range>
        using range_range_reference_t = typename range_range_reference<Range>::type;
    }
}

#endif // BURST_ITERATOR_DETAIL_RANGE_RANGE_HPP
