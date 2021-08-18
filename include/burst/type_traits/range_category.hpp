#ifndef BURST__TYPE_TRAITS__RANGE_CATEGORY_HPP
#define BURST__TYPE_TRAITS__RANGE_CATEGORY_HPP

#include <boost/range/category.hpp>

namespace burst
{
    template <typename Range>
    using range_category_t = typename boost::range_category<Range>::type;
} // namespace burst

#endif // BURST__TYPE_TRAITS__RANGE_CATEGORY_HPP
