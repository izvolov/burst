#ifndef BURST__ITERATOR__VARIANT_RANGE_HPP
#define BURST__ITERATOR__VARIANT_RANGE_HPP

#include <burst/iterator/variant_iterator.hpp>
#include <burst/type_traits/range_iterator.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    template <typename... Ranges>
    using variant_range = boost::iterator_range<variant_iterator<range_iterator_t<Ranges>...>>;
} // namespace burst

#endif // BURST__ITERATOR__VARIANT_RANGE_HPP
